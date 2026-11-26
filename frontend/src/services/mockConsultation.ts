import { demoCategories, demoMedia, sampleDecisions } from '../data/demo'
import type { ConsultationSession, MediaCategory, Recommendation } from '../types/consultation'
import { validateProfile } from './consultation'
import type { ConsultationService } from './consultation'

export function createMockConsultation(): ConsultationService {
  const sessions = new Map<string, { session: ConsultationSession; queue: MediaCategory[]; sample: boolean }>()
  let nextId = 0

  function active(id: string) {
    const state = sessions.get(id)
    if (!state || state.session.status !== 'active') throw new Error('This consultation is no longer active.')
    return state
  }

  function decision(category: MediaCategory | undefined, sample?: Recommendation): Recommendation {
    if (!category) return {
      recommendedMediaType: 'manual_intervention', firedRules: [],
      rationale: 'The simulated preference list is exhausted. The physician can end the consultation.',
    }
    return {
      recommendedMediaType: category, firedRules: sample?.firedRules ?? [],
      rationale: sample?.rationale ?? 'The demo selected this category from the ordered media preferences.',
      item: demoMedia[category],
    }
  }

  return {
    async start(profile) {
      const error = Object.values(validateProfile(profile)).find(Boolean)
      if (error) throw new Error(error)
      const queue = profile.preferences.map((preference) => demoCategories[preference])
      const sample = profile.age === 2 && profile.preferences.join(',') === 'visuals,video,audio'
      const session: ConsultationSession = {
        id: String(++nextId), status: 'active', feedback: [],
        history: [decision(queue[0], sample ? sampleDecisions[0] : undefined)],
      }
      sessions.set(session.id, { session, queue, sample })
      return session
    },
    async submitFeedback(id, value) {
      const state = active(id)
      if (!['effective', 'ineffective', 'disruptive'].includes(value)) throw new Error('Unsupported feedback.')
      const { session, queue } = state
      const current = session.history.at(-1)
      if (!current || current.recommendedMediaType === 'manual_intervention') throw new Error('There is no activity to evaluate.')
      const sampleStep = state.sample && session.feedback.length === 0 && value === 'ineffective'
        ? sampleDecisions[1]
        : state.sample && session.feedback.length === 1 && session.feedback[0].value === 'ineffective' && value === 'disruptive'
          ? sampleDecisions[2]
          : undefined
      if (value !== 'effective') queue.shift()
      const next = value === 'effective'
        ? { ...current, firedRules: [], rationale: 'The physician marked the activity effective. The demo is keeping it.' }
        : decision(queue[0], sampleStep)
      state.session = {
        ...session, history: [...session.history, next],
        feedback: [...session.feedback, { value, previousCategory: current.recommendedMediaType }],
      }
      return state.session
    },
    async end(id) {
      const state = active(id)
      state.session = { ...state.session, status: 'ended' }
      return state.session
    },
  }
}
