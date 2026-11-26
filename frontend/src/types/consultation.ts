export const preferenceLabels = {
  visuals: 'Images',
  video: 'Video',
  audio: 'Music / audio',
  interaction: 'Interactive games',
} as const

export type MediaPreference = keyof typeof preferenceLabels
export type MediaCategory = 'images' | 'video' | 'music' | 'games'
export type Feedback = 'effective' | 'ineffective' | 'disruptive'

export interface PatientProfile {
  age: number
  interests: string[]
  preferences: MediaPreference[]
}

export interface MultimediaItem {
  title: string
  kind: 'image' | 'video' | 'audio'
  url: string
  description: string
}

export interface Recommendation {
  recommendedMediaType: MediaCategory | 'manual_intervention'
  rationale: string
  firedRules: string[]
  item?: MultimediaItem
}

export interface PhysicianFeedback {
  value: Feedback
  previousCategory: Recommendation['recommendedMediaType']
}

export interface ConsultationSession {
  id: string
  status: 'active' | 'ended'
  history: Recommendation[]
  feedback: PhysicianFeedback[]
}
