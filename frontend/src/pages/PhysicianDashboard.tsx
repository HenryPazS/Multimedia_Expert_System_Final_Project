import { MediaPreview } from '../components/MediaPreview'
import type { ConsultationSession, Feedback, Recommendation } from '../types/consultation'

const categories: Record<Recommendation['recommendedMediaType'], string> = {
  images: 'Images', video: 'Video', music: 'Music / audio', games: 'Interactive games',
  manual_intervention: 'Manual intervention suggested',
}

const feedbackLabels: Record<Feedback, string> = {
  effective: 'Effective', ineffective: 'Ineffective', disruptive: 'Disruptive',
}

interface Props {
  session?: ConsultationSession
  pending: boolean
  onFeedback: (feedback: Feedback) => void
  onEnd: () => void
}

export function PhysicianDashboard({ session, pending, onFeedback, onEnd }: Props) {
  const current = session?.history.at(-1)
  const lastFeedback = session?.feedback.at(-1)
  const active = session?.status === 'active'

  return <section aria-labelledby="dashboard-title">
    <h1 id="dashboard-title">Physician dashboard</h1>
    <p role="status" className="session-status">{!session ? 'No consultation started' : active ? 'Consultation active' : 'Consultation ended'}{pending && ' · Saving changes…'}</p>
    {!session ? <p>Start a consultation in Parent setup to see a recommendation.</p> : <>
      <div className="dashboard">
        <section className="panel" aria-labelledby="recommendation-title">
          <h2 id="recommendation-title">Current recommendation</h2>
          {current ? <>
            <h3>{categories[current.recommendedMediaType]}</h3>
            <p>{current.rationale}</p>
            <h3>Rules fired</h3>
            {current.firedRules.length ? <ul>{current.firedRules.map((rule) => <li key={rule}>{rule}</li>)}</ul> : <p>No rule identifiers were returned.</p>}
          </> : <p>No recommendation is available.</p>}
        </section>
        {active && <MediaPreview key={current?.item?.url} item={current?.item} />}
        <section className="panel" aria-labelledby="feedback-title">
          <h2 id="feedback-title">Physician feedback</h2>
          <p>{lastFeedback ? `Latest feedback: ${feedbackLabels[lastFeedback.value]} for ${categories[lastFeedback.previousCategory]}.` : 'No feedback submitted yet.'}</p>
          {active && <>
            {current && current.recommendedMediaType !== 'manual_intervention' && <>
              <p>Select Ineffective to request the next recommendation.</p>
              <div className="actions">
                {(Object.keys(feedbackLabels) as Feedback[]).map((value) =>
                  <button key={value} disabled={pending} onClick={() => onFeedback(value)}>{feedbackLabels[value]}</button>)}
              </div>
            </>}
            <p>Manual category selection requires backend support.</p>
            <button disabled={pending} onClick={onEnd}>End consultation</button>
          </>}
        </section>
        <section className="panel" aria-labelledby="history-title">
          <h2 id="history-title">Recommendation history</h2>
          {session.history.length ? <ol>{session.history.map((entry, index) =>
            <li key={index}>{categories[entry.recommendedMediaType]}</li>
          )}</ol> : <p>No recommendations recorded.</p>}
        </section>
      </div>
    </>}
  </section>
}
