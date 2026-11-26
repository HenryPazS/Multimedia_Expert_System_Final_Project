import { useRef, useState } from 'react'
import { ParentSetup } from './pages/ParentSetup'
import { PhysicianDashboard } from './pages/PhysicianDashboard'
import type { ConsultationService } from './services/consultation'
import { createMockConsultation } from './services/mockConsultation'
import type { ConsultationSession } from './types/consultation'
import './App.css'

type View = 'setup' | 'dashboard'

export default function App({ service }: { service?: ConsultationService }) {
  const [demoService] = useState(createMockConsultation)
  const adapter = service ?? demoService
  const [view, setView] = useState<View>('setup')
  const [session, setSession] = useState<ConsultationSession>()
  const [pending, setPending] = useState(false)
  const [error, setError] = useState('')
  const requestInProgress = useRef(false)

  async function update(request: () => Promise<ConsultationSession>, nextView?: View) {
    if (requestInProgress.current) return
    requestInProgress.current = true
    setPending(true)
    setError('')
    try {
      setSession(await request())
      if (nextView) setView(nextView)
    } catch (failure) {
      setError(failure instanceof Error ? failure.message : 'The request failed. Please try again.')
    } finally {
      requestInProgress.current = false
      setPending(false)
    }
  }

  return <>
    <header>
      <div className="header-content">
        <div className="brand">Adaptive Child Distraction</div>
        {!service && <span className="mode">Demo mode · Simulated recommendations</span>}
        <nav aria-label="Consultation">
          <button aria-current={view === 'setup' ? 'page' : undefined} disabled={pending} onClick={() => setView('setup')}>Parent setup</button>
          <button aria-current={view === 'dashboard' ? 'page' : undefined} disabled={pending} onClick={() => setView('dashboard')}>Physician dashboard</button>
        </nav>
      </div>
    </header>
    <main>
      {error && <p className="error" role="alert">{error}</p>}
      <div hidden={view !== 'setup'}>
        {session?.status === 'active'
          ? <p>End the active consultation in the physician dashboard before starting another.</p>
          : <ParentSetup pending={pending} onStart={(profile) => void update(() => adapter.start(profile), 'dashboard')} />}
      </div>
      {view === 'dashboard' && <PhysicianDashboard session={session} pending={pending}
        onFeedback={(feedback) => { if (session) void update(() => adapter.submitFeedback(session.id, feedback)) }}
        onEnd={() => { if (session) void update(() => adapter.end(session.id)) }} />}
    </main>
  </>
}
