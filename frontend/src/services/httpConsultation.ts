import type { ConsultationSession, Recommendation, PhysicianFeedback, MultimediaItem } from '../types/consultation'
import { validateProfile } from './consultation'
import type { ConsultationService } from './consultation'

const categories = ['images', 'video', 'music', 'games', 'manual_intervention']
const feedbackValues = ['effective', 'ineffective', 'disruptive']

function record(value: unknown): value is Record<string, unknown> {
  return typeof value === 'object' && value !== null && !Array.isArray(value)
}

function oneOf(value: unknown, values: string[]) {
  return typeof value === 'string' && values.includes(value)
}

function validItem(item: unknown): item is MultimediaItem {
  return record(item) && oneOf(item.kind, ['image', 'video', 'audio']) &&
    typeof item.title === 'string' && typeof item.description === 'string' &&
    typeof item.url === 'string' && /^(https?:\/\/[^/]|\/(?!\/))/.test(item.url)
}

function validDecision(decision: unknown): decision is Recommendation {
  return record(decision) && oneOf(decision.recommendedMediaType, categories) &&
    typeof decision.rationale === 'string' && Array.isArray(decision.firedRules) &&
    decision.firedRules.every((rule) => typeof rule === 'string') &&
    (decision.item === undefined || validItem(decision.item))
}

function validFeedback(entry: unknown): entry is PhysicianFeedback {
  return record(entry) && oneOf(entry.value, feedbackValues) && oneOf(entry.previousCategory, categories)
}

function validSession(session: unknown): session is ConsultationSession {
  return record(session) && typeof session.id === 'string' && session.id.length > 0 &&
    oneOf(session.status, ['active', 'ended']) &&
    Array.isArray(session.history) && session.history.every(validDecision) &&
    Array.isArray(session.feedback) && session.feedback.every(validFeedback)
}

export function createHttpConsultation(baseUrl = '/api/consultations', fetcher = fetch): ConsultationService {
  async function post(path: string, status: ConsultationSession['status'], body?: unknown, id?: string) {
    const response = await fetcher(baseUrl + path, {
      method: 'POST',
      ...(body === undefined ? {} : {
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body),
      }),
    }).catch(() => { throw new Error('Could not reach the consultation server. Please try again.') })

    const payload: unknown = await response.json().catch(() => undefined)
    if (!response.ok) {
      const message = record(payload) && typeof payload.message === 'string' && payload.message
      throw new Error(message || `Consultation request failed (HTTP ${response.status}).`)
    }
    if (!validSession(payload) || payload.status !== status || (id !== undefined && payload.id !== id)) {
      throw new Error('The consultation server returned an invalid session.')
    }
    return payload
  }

  return {
    async start(profile) {
      const error = Object.values(validateProfile(profile)).find(Boolean)
      if (error) throw new Error(error)
      return post('', 'active', profile)
    },
    async submitFeedback(id, value) {
      if (!oneOf(value, feedbackValues)) throw new Error('Unsupported feedback.')
      return post(`/${encodeURIComponent(id)}/feedback`, 'active', { value }, id)
    },
    end(id) {
      return post(`/${encodeURIComponent(id)}/end`, 'ended', undefined, id)
    },
  }
}
