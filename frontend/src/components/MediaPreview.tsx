import { useState } from 'react'
import type { MultimediaItem } from '../types/consultation'

export function MediaPreview({ item }: { item?: MultimediaItem }) {
  const [failed, setFailed] = useState(false)

  return <section className="panel" aria-labelledby="media-title">
    <h2 id="media-title">Multimedia</h2>
    {item && !failed ? <figure>
      {item.kind === 'image' && <img src={item.url} alt={item.description} onError={() => setFailed(true)} />}
      {item.kind === 'video' && <video src={item.url} controls playsInline preload="metadata" aria-label={item.title} onError={() => setFailed(true)} />}
      {item.kind === 'audio' && <audio src={item.url} controls preload="metadata" aria-label={item.title} onError={() => setFailed(true)} />}
      <figcaption>{item.title}{item.kind !== 'image' && <p>{item.description}</p>}</figcaption>
    </figure> : <p>{failed ? 'The media could not be loaded.' : 'No multimedia item is available for this recommendation.'}</p>}
  </section>
}
