import { describe, expect, it, vi } from 'vitest'
import { createHttpConsultation } from '../services/httpConsultation'
import { createMockConsultation } from '../services/mockConsultation'
import type { Feedback, MediaPreference, PatientProfile } from '../types/consultation'

const profile: PatientProfile = { age: 2, interests: ['animals', 'colors'], preferences: ['visuals', 'video', 'audio'] }
const emptySession = { id: 'demo/1', status: 'active', history: [], feedback: [] }
const json = (body: unknown, status = 200) => new Response(JSON.stringify(body), { status })

describe('simulated consultation', () => {
  it('shows the recorded C++ sample sequence and keeps the activity after effective feedback', async () => {
    const service = createMockConsultation()
    const initial = await service.start(profile)
    const next = await service.submitFeedback(initial.id, 'ineffective')
    const fallback = await service.submitFeedback(initial.id, 'disruptive')
    const retained = await service.submitFeedback(initial.id, 'effective')
    expect(initial.history).toHaveLength(1)
    expect(next.history.at(-1)?.firedRules).toEqual(['Rule 8'])
    expect(fallback.history.map((entry) => entry.recommendedMediaType)).toEqual(['images', 'video', 'music'])
    expect(fallback.history.at(-1)?.firedRules).toEqual(['Rule 3', 'Rule 4'])
    expect(retained.history.at(-1)?.recommendedMediaType).toBe('music')
  })

  it('handles missing media, exhausted preferences, and a closed session', async () => {
    const service = createMockConsultation()
    const initial = await service.start({ ...profile, preferences: ['interaction'] })
    expect(initial.history[0].recommendedMediaType).toBe('games')
    expect(initial.history[0].item).toBeUndefined()
    const next = await service.submitFeedback(initial.id, 'disruptive')
    expect(next.history.at(-1)?.recommendedMediaType).toBe('manual_intervention')
    await expect(service.submitFeedback(initial.id, 'effective')).rejects.toThrow('no activity')
    await expect(service.end(initial.id)).resolves.toMatchObject({ status: 'ended' })
    await expect(service.submitFeedback(initial.id, 'effective')).rejects.toThrow('no longer active')
  })

  it('rejects unsupported inputs without altering the current recommendation', async () => {
    const service = createMockConsultation()
    await expect(service.start({ ...profile, age: Infinity })).rejects.toThrow('Enter an age')
    await expect(service.start({ ...profile, preferences: ['unknown' as MediaPreference] })).rejects.toThrow('supported media')
    const initial = await service.start(profile)
    await expect(service.submitFeedback(initial.id, 'unknown' as Feedback)).rejects.toThrow('Unsupported feedback')
    const next = await service.submitFeedback(initial.id, 'ineffective')
    expect(next.feedback).toHaveLength(1)
    expect(next.history.at(-1)?.recommendedMediaType).toBe('video')
  })
})

describe('proposed HTTP contract', () => {
  it('creates and ends a session, accepting an empty recommendation history', async () => {
    const fetcher = vi.fn<typeof fetch>()
      .mockResolvedValueOnce(json(emptySession, 201))
      .mockResolvedValueOnce(json({ ...emptySession, status: 'ended' }))
    const service = createHttpConsultation(undefined, fetcher)
    await expect(service.start(profile)).resolves.toEqual(emptySession)
    expect(fetcher).toHaveBeenNthCalledWith(1, '/api/consultations', {
      method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(profile),
    })
    await expect(service.end(emptySession.id)).resolves.toMatchObject({ status: 'ended' })
    expect(fetcher).toHaveBeenNthCalledWith(2, '/api/consultations/demo%2F1/end', { method: 'POST' })
  })

  it('reports a failed feedback request and accepts a subsequent successful request', async () => {
    const updated = { ...emptySession, history: [{ recommendedMediaType: 'video', firedRules: ['Rule 8'], rationale: 'Next preference.' }] }
    const fetcher = vi.fn<typeof fetch>()
      .mockResolvedValueOnce(json({ message: 'Feedback temporarily unavailable.' }, 503))
      .mockResolvedValueOnce(json(updated))
    const service = createHttpConsultation(undefined, fetcher)
    await expect(service.submitFeedback(emptySession.id, 'ineffective')).rejects.toThrow('Feedback temporarily unavailable.')
    await expect(service.submitFeedback(emptySession.id, 'ineffective')).resolves.toEqual(updated)
    expect(fetcher).toHaveBeenLastCalledWith('/api/consultations/demo%2F1/feedback', {
      method: 'POST', headers: { 'Content-Type': 'application/json' }, body: '{"value":"ineffective"}',
    })
  })

  it.each([
    { ...emptySession, history: [{ recommendedMediaType: 'unknown' }] },
    { ...emptySession, feedback: [{ value: 'unsupported', previousCategory: 'images' }] },
    { ...emptySession, status: 'ended' },
    null,
  ])('rejects invalid success response %#', async (payload) => {
    const service = createHttpConsultation(undefined, vi.fn<typeof fetch>().mockResolvedValue(json(payload)))
    await expect(service.start(profile)).rejects.toThrow('invalid session')
  })

  it('provides fallback messages for network and non-JSON server errors', async () => {
    const fetcher = vi.fn<typeof fetch>().mockRejectedValueOnce(new TypeError('Offline'))
      .mockResolvedValueOnce(new Response('Internal error', { status: 500 }))
    const service = createHttpConsultation(undefined, fetcher)
    await expect(service.start(profile)).rejects.toThrow('Could not reach')
    await expect(service.start(profile)).rejects.toThrow('HTTP 500')
  })
})
