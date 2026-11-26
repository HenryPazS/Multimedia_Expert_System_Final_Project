import { useState } from 'react'
import type { FormEvent } from 'react'
import { preferenceLabels } from '../types/consultation'
import type { MediaPreference, PatientProfile } from '../types/consultation'
import { validateProfile } from '../services/consultation'

const options = Object.keys(preferenceLabels) as MediaPreference[]

interface Props {
  onStart: (profile: PatientProfile) => void
  pending: boolean
}

export function ParentSetup({ onStart, pending }: Props) {
  const [age, setAge] = useState('')
  const [interests, setInterests] = useState('')
  const [preferences, setPreferences] = useState<MediaPreference[]>([])
  const [submitted, setSubmitted] = useState(false)

  const profile = {
    age: age.trim() ? Number(age) : NaN,
    interests: interests.split(',').map((interest) => interest.trim()).filter(Boolean),
    preferences,
  }
  const errors = validateProfile(profile)

  function submit(event: FormEvent) {
    event.preventDefault()
    setSubmitted(true)
    if (!pending && !Object.values(errors).some(Boolean)) onStart(profile)
  }

  function move(index: number, offset: number) {
    const next = [...preferences]
    const destination = index + offset
    if (destination < 0 || destination >= next.length) return
    const [preference] = next.splice(index, 1)
    next.splice(destination, 0, preference)
    setPreferences(next)
  }

  return (
    <section className="panel" aria-labelledby="setup-title">
      <h1 id="setup-title">Parent setup</h1>
      <p>Enter the child’s age and interests, then rank the media they prefer.</p>
      <form noValidate onSubmit={submit}>
        <fieldset disabled={pending}>
          <label htmlFor="age">Child’s age in years</label>
          <input id="age" type="number" min="0" step="any" required value={age}
            aria-invalid={submitted && !!errors.age} aria-describedby={submitted && errors.age ? 'age-error' : undefined}
            onChange={(event) => setAge(event.target.value)} />
          {submitted && errors.age && <p className="error" id="age-error" role="alert">{errors.age}</p>}
          <label htmlFor="interests">Child’s interests</label>
          <input id="interests" required value={interests} placeholder="Animals, colors"
            aria-invalid={submitted && !!errors.interests} aria-describedby={submitted && errors.interests ? 'interests-error' : undefined}
            onChange={(event) => setInterests(event.target.value)} />
          {submitted && errors.interests && <p className="error" id="interests-error" role="alert">{errors.interests}</p>}
          <fieldset className="choices">
            <legend>Preferred media</legend>
            {options.map((option) => (
              <label key={option}>
                <input type="checkbox" checked={preferences.includes(option)} onChange={(event) =>
                  setPreferences(event.target.checked ? [...preferences, option] : preferences.filter((item) => item !== option))
                } />
                {preferenceLabels[option]}
              </label>
            ))}
            {submitted && errors.preferences && <p className="error" role="alert">{errors.preferences}</p>}
          </fieldset>
          <h2>Preference order</h2>
          {preferences.length ? <ol className="preference-list" aria-label="Preference order">
            {preferences.map((preference, index) => (
              <li key={preference}>
                <span>{preferenceLabels[preference]}</span>
                <button type="button" aria-label={`Move ${preferenceLabels[preference]} up`} disabled={index === 0} onClick={() => move(index, -1)}>Up</button>
                <button type="button" aria-label={`Move ${preferenceLabels[preference]} down`} disabled={index === preferences.length - 1} onClick={() => move(index, 1)}>Down</button>
              </li>
            ))}
          </ol> : <p>Select media to set the order.</p>}
          <button type="submit" className="primary">{pending ? 'Starting…' : 'Start consultation'}</button>
        </fieldset>
      </form>
    </section>
  )
}
