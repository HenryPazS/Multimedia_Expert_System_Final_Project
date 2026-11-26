import { preferenceLabels } from '../types/consultation'
import type { ConsultationSession, Feedback, PatientProfile } from '../types/consultation'

export interface ConsultationService {
  start(profile: PatientProfile): Promise<ConsultationSession>
  submitFeedback(sessionId: string, feedback: Feedback): Promise<ConsultationSession>
  end(sessionId: string): Promise<ConsultationSession>
}

export function validateProfile(profile: PatientProfile) {
  return {
    age: Number.isFinite(profile.age) && profile.age >= 0 ? '' : 'Enter an age of zero or more years.',
    interests: profile.interests.some((interest) => interest.trim()) ? '' : 'Enter at least one interest.',
    preferences: profile.preferences.length &&
      profile.preferences.every((preference) => Object.hasOwn(preferenceLabels, preference)) &&
      new Set(profile.preferences).size === profile.preferences.length
      ? '' : 'Select at least one supported media type. Each type can appear once.',
  }
}
