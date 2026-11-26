// @vitest-environment jsdom
import { cleanup, fireEvent, render, screen, within } from '@testing-library/react'
import userEvent from '@testing-library/user-event'
import { afterEach, expect, it, vi } from 'vitest'
import App from '../App'
import { MediaPreview } from '../components/MediaPreview'
import { createMockConsultation } from '../services/mockConsultation'

afterEach(cleanup)

it('shows required-field errors and rejects a negative age', async () => {
  const user = userEvent.setup()
  render(<App />)
  await user.click(screen.getByRole('button', { name: 'Start consultation' }))
  expect(screen.getAllByRole('alert')).toHaveLength(3)
  await user.type(screen.getByLabelText('Child’s age in years'), '-2')
  await user.type(screen.getByLabelText('Child’s interests'), 'Animals')
  await user.click(screen.getByRole('checkbox', { name: 'Images' }))
  await user.click(screen.getByRole('button', { name: 'Start consultation' }))
  expect(screen.getByRole('alert').textContent).toContain('zero or more years')
})

it('submits the selected media in the chosen order', async () => {
  const user = userEvent.setup()
  const service = createMockConsultation()
  const start = vi.spyOn(service, 'start')
  render(<App service={service} />)
  await user.type(screen.getByLabelText('Child’s age in years'), '2.5')
  await user.type(screen.getByLabelText('Child’s interests'), ' Animals, colors ')
  await user.click(screen.getByRole('checkbox', { name: 'Video' }))
  await user.click(screen.getByRole('checkbox', { name: 'Images' }))
  await user.click(screen.getByRole('button', { name: 'Move Images up' }))
  await user.click(screen.getByRole('button', { name: 'Start consultation' }))
  expect(start).toHaveBeenCalledWith({ age: 2.5, interests: ['Animals', 'colors'], preferences: ['visuals', 'video'] })
  expect((await screen.findByRole('status')).textContent).toBe('Consultation active')
})

it('shows the sample recommendations, rationale and rules, tracks feedback, and ends the session', async () => {
  const user = userEvent.setup()
  render(<App />)
  expect(screen.getByText('Demo mode · Simulated recommendations')).toBeTruthy()
  await user.click(screen.getByRole('button', { name: 'Physician dashboard' }))
  expect(screen.getByRole('status').textContent).toBe('No consultation started')
  await user.click(screen.getByRole('button', { name: 'Parent setup' }))
  await user.type(screen.getByLabelText('Child’s age in years'), '2')
  await user.type(screen.getByLabelText('Child’s interests'), 'Animals, colors')
  for (const name of ['Images', 'Video', 'Music / audio']) {
    await user.click(screen.getByRole('checkbox', { name }))
  }
  await user.click(screen.getByRole('button', { name: 'Start consultation' }))
  const recommendation = await screen.findByRole('region', { name: 'Current recommendation' })
  expect(within(recommendation).getByRole('heading', { name: 'Images' })).toBeTruthy()
  expect(within(recommendation).getByText(/very young/)).toBeTruthy()
  expect(within(recommendation).getByText('Rule 1')).toBeTruthy()
  expect(screen.getByRole('img', { name: /colored squares/ })).toBeTruthy()
  await user.click(screen.getByRole('button', { name: 'Ineffective' }))
  expect(await within(recommendation).findByRole('heading', { name: 'Video' })).toBeTruthy()
  expect(within(recommendation).getByText('Rule 8')).toBeTruthy()
  expect(screen.getByLabelText('Demo video · Moving circle').hasAttribute('autoplay')).toBe(false)
  await user.click(screen.getByRole('button', { name: 'Disruptive' }))
  expect(await within(recommendation).findByRole('heading', { name: 'Music / audio' })).toBeTruthy()
  expect(screen.getByText('Latest feedback: Disruptive for Video.')).toBeTruthy()
  expect(within(recommendation).getByText('Rule 4')).toBeTruthy()
  const history = screen.getByRole('region', { name: 'Recommendation history' })
  expect(within(history).getAllByRole('listitem').map((item) => item.textContent)).toEqual(['Images', 'Video', 'Music / audio'])
  expect(screen.getByLabelText('Demo audio · Soft tone').hasAttribute('autoplay')).toBe(false)
  await user.click(screen.getByRole('button', { name: 'End consultation' }))
  expect(screen.getByRole('status').textContent).toBe('Consultation ended')
  expect(screen.queryByRole('button', { name: 'Effective' })).toBeNull()
  expect(screen.queryByLabelText('Demo audio · Soft tone')).toBeNull()
})

it('renders missing media and handles a failed media load', () => {
  const { rerender } = render(<MediaPreview />)
  expect(screen.getByText(/No multimedia item/)).toBeTruthy()
  rerender(<MediaPreview item={{ title: 'Demo image', url: '/missing.png', kind: 'image', description: 'Placeholder' }} />)
  fireEvent.error(screen.getByRole('img', { name: 'Placeholder' }))
  expect(screen.getByText('The media could not be loaded.')).toBeTruthy()
})
