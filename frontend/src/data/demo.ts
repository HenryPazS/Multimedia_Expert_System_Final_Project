import type { MediaCategory, MediaPreference, MultimediaItem, Recommendation } from '../types/consultation'

export const demoCategories: Record<MediaPreference, MediaCategory> = {
  visuals: 'images', video: 'video', audio: 'music', interaction: 'games',
}

export const demoMedia: Partial<Record<MediaCategory, MultimediaItem>> = {
  images: { title: 'Demo image · Color blocks', kind: 'image', url: import.meta.env.BASE_URL + 'demo/colors.png', description: 'Three colored squares on a pale background.' },
  video: { title: 'Demo video · Moving circle', kind: 'video', url: import.meta.env.BASE_URL + 'demo/moving-circle.mp4', description: 'A silent animation of a circle moving across a grid.' },
  music: { title: 'Demo audio · Soft tone', kind: 'audio', url: import.meta.env.BASE_URL + 'demo/tone.mp3', description: 'A short synthesized tone that fades in and out.' },
}

export const sampleDecisions: Recommendation[] = [
  { recommendedMediaType: 'images', firedRules: ['Rule 1'], rationale: 'The sample child is very young and prefers visuals, so the example starts with simple images.' },
  { recommendedMediaType: 'video', firedRules: ['Rule 8'], rationale: 'The images were ineffective. Video is next in the sample preference list.' },
  { recommendedMediaType: 'music', firedRules: ['Rule 3', 'Rule 4'], rationale: 'The video was disruptive. The sample removes video and suggests music.' },
]
