import React, { useRef } from 'react';
import Histogram from './Histogram';
import RGBHistogram from './RGBHistogram';
import CDFChart from './CDFChart';

/* ─── Save helpers ─────────────────────────────────────────────────────────── */
function saveBase64Image(base64, filename = 'output.png') {
    const a = document.createElement('a');
    a.href = `data:image/png;base64,${base64}`;
    a.download = filename;
    a.click();
}

function saveCanvasFromContainer(containerRef, filename = 'plot.png') {
    const canvases = Array.from(containerRef.current?.querySelectorAll('canvas') ?? []);
    if (canvases.length === 0) return;

    const GAP = 8;
    const totalW = Math.max(...canvases.map(c => c.width));
    const totalH = canvases.reduce((sum, c) => sum + c.height, 0) + GAP * (canvases.length - 1);

    const offscreen = document.createElement('canvas');
    offscreen.width = totalW;
    offscreen.height = totalH;
    const ctx = offscreen.getContext('2d');
    ctx.fillStyle = '#fff';
    ctx.fillRect(0, 0, totalW, totalH);

    let y = 0;
    canvases.forEach(c => { ctx.drawImage(c, 0, y); y += c.height + GAP; });

    offscreen.toBlob(blob => {
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url; a.download = filename; a.click();
        URL.revokeObjectURL(url);
    });
}

/* ─── Upload slot ──────────────────────────────────────────────────────────── */
function UploadSlot({ label, preview, onUpload }) {
    const fileRef = useRef();
    const drop = (e) => { e.preventDefault(); const f = e.dataTransfer.files[0]; if (f) onUpload(f); };
    return (
        <div className={`image-slot${preview ? ' has-image' : ''}`}
            onClick={() => fileRef.current.click()} onDragOver={e => e.preventDefault()} onDrop={drop}>
            <input ref={fileRef} type="file" accept="image/*" style={{ display: 'none' }}
                onChange={e => e.target.files[0] && onUpload(e.target.files[0])} />
            {preview ? (
                <>
                    <img src={preview} alt={`${label} input`} className="slot-img" />
                    <div className="slot-overlay">
                        <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="white" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
                            <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4" />
                            <polyline points="17 8 12 3 7 8" /><line x1="12" y1="3" x2="12" y2="15" />
                        </svg>
                        <span>Replace</span>
                    </div>
                </>
            ) : (
                <div className="slot-placeholder">
                    <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5" strokeLinecap="round" strokeLinejoin="round">
                        <rect x="3" y="3" width="18" height="18" rx="2" /><circle cx="8.5" cy="8.5" r="1.5" />
                        <polyline points="21 15 16 10 5 21" />
                    </svg>
                    <span>{label}</span>
                </div>
            )}
        </div>
    );
}

/* ─── Output slot ──────────────────────────────────────────────────────────── */
function OutputSlot({ label, histEntry, chartEntry, histAfter, containerRef }) {
    // chartEntry is always the most recent action (cleared by any subsequent edit op)
    const rgbHist = chartEntry?.rgbHist ?? null;
    const cdfData = chartEntry?.cdfData ?? null;
    const isChart = !!(rgbHist || cdfData);
    const result = isChart ? null : (histEntry?.result ?? null);
    const hasContent = result || rgbHist || cdfData;


    return (
        <div className="output-slot-wrapper">
            <div ref={containerRef} className={`image-slot output-slot${hasContent ? ' has-image' : ''}`}>
                {result ? (
                    <img src={`data:image/png;base64,${result}`} alt={`${label} output`} className="slot-img" />
                ) : rgbHist ? (
                    <div className="rgb-hist-output"><RGBHistogram histR={rgbHist.r} histG={rgbHist.g} histB={rgbHist.b} /></div>
                ) : cdfData ? (
                    <div className="rgb-hist-output"><CDFChart cdfR={cdfData.r} cdfG={cdfData.g} cdfB={cdfData.b} /></div>
                ) : (
                    <div className="slot-placeholder output">
                        <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.5" strokeLinecap="round" strokeLinejoin="round">
                            <polyline points="22 12 18 12 15 21 9 3 6 12 2 12" />
                        </svg>
                        <span>Output will appear here</span>
                    </div>
                )}
            </div>

            {/* AFTER histogram aligned under the output slot */}
            {histAfter && <Histogram data={histAfter} title="AFTER" />}
        </div>
    );
}

/* ─── Arrow ────────────────────────────────────────────────────────────────── */
function Arrow() {
    return (
        <div className="slot-arrow">
            <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
                <line x1="5" y1="12" x2="19" y2="12" /><polyline points="12 5 19 12 12 19" />
            </svg>
        </div>
    );
}

/* ─── History breadcrumb ───────────────────────────────────────────────────── */
function HistoryBar({ history, chartResult, onUndo, onReset, onSave, saveLabel }) {
    const hasHistory = history.length > 0;
    if (!hasHistory && !chartResult) return null;
    return (
        <div className="history-bar">
            <div className="history-breadcrumb">
                <span className="history-step orig">Original</span>
                {history.map((e, i) => (
                    <React.Fragment key={i}>
                        <span className="history-arrow">›</span>
                        <span className={`history-step${i === history.length - 1 ? ' current' : ''}`}>{e.opLabel}</span>
                    </React.Fragment>
                ))}
                {chartResult && (
                    <>
                        {hasHistory && <span className="history-arrow">·</span>}
                        <span className="history-step" style={{ background: '#f3f0ff', color: '#7c3aed' }}>
                            {chartResult.opLabel}
                        </span>
                    </>
                )}
            </div>
            <div className="history-actions">
                {onSave && (
                    <button className="hist-btn save-btn-inline" onClick={onSave} title={saveLabel}>
                        <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.2" strokeLinecap="round" strokeLinejoin="round">
                            <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4" />
                            <polyline points="7 10 12 15 17 10" /><line x1="12" y1="15" x2="12" y2="3" />
                        </svg>
                        {saveLabel}
                    </button>
                )}
                <div style={{ width: 1, height: 20, background: 'var(--border)', margin: '0 4px' }} />
                <button className="hist-btn undo-btn" onClick={onUndo}>↩ Undo</button>
                <button className="hist-btn reset-btn" onClick={onReset}>✕ Reset</button>
            </div>
        </div>
    );
}

function ImageRow({ label, preview, history, chartResult, onUpload, onUndo, onReset, operation }) {
    const containerRef = useRef();
    const current = history.at(-1) ?? null;
    const isChart = !!(chartResult?.rgbHist || chartResult?.cdfData);

    // histBefore under input slot, histAfter in output column
    const histBefore = isChart ? null : (current?.histBefore ?? null);
    const histAfter = isChart ? null : (current?.histAfter ?? null);

    // For analysis ops, show last edited image in input slot
    const ANALYSIS = ['histogram', 'cdf'];
    const lastResult = ANALYSIS.includes(operation) ? [...history].reverse().find(h => h.result) : null;
    const effectivePreview = lastResult ? `data:image/png;base64,${lastResult.result}` : preview;

    const hasSave = !!(current?.result || chartResult?.rgbHist || chartResult?.cdfData);
    const handleSave = () => {
        if (!hasSave) return;
        if (isChart) saveCanvasFromContainer(containerRef, `${label.replace(' ', '_')}_plot.png`);
        else saveBase64Image(current.result, `${label.replace(' ', '_')}_output.png`);
    };

    return (
        <div className="image-row">
            <div className="image-row-label">{label}</div>
            <div className="image-row-panels">
                {/* Input column — upload slot + BEFORE histogram below */}
                <div className="input-col">
                    <UploadSlot label="Click or drop to upload" preview={effectivePreview} onUpload={onUpload} />
                    {histBefore && <Histogram data={histBefore} title="BEFORE" />}
                </div>
                <Arrow />
                {/* Output column — slot + AFTER histogram below */}
                <OutputSlot label={label} histEntry={current} chartEntry={chartResult} histAfter={histAfter} containerRef={containerRef} />
            </div>
            <HistoryBar
                history={history} chartResult={chartResult}
                onUndo={onUndo} onReset={onReset}
                onSave={hasSave ? handleSave : null}
                saveLabel={isChart ? 'Save Plot' : 'Save Output'}
            />
        </div>
    );
}

/* ─── Hybrid layout ────────────────────────────────────────────────────────── */
function HybridLayout({ preview1, onUpload1, preview2, onUpload2, history1, history2, hybridResult }) {
    const containerRef = useRef();
    // The inputs should display the latest edited image (Hybrid is no longer in history, so we just take the last result)
    const result1 = [...history1].reverse().find(h => h.result);
    const result2 = [...history2].reverse().find(h => h.result);
    const effPrev1 = result1 ? `data:image/png;base64,${result1.result}` : preview1;
    const effPrev2 = result2 ? `data:image/png;base64,${result2.result}` : preview2;

    const hybridEntry = hybridResult ? { result: hybridResult } : null;

    return (
        <div className="hybrid-layout">
            <div className="hybrid-inputs">
                <div className="hybrid-input-label">Image 1 (Low-pass)</div>
                <UploadSlot label="Click or drop Image 1" preview={effPrev1} onUpload={onUpload1} />
                <div className="hybrid-input-label" style={{ marginTop: 10 }}>Image 2 (High-pass)</div>
                <UploadSlot label="Click or drop Image 2" preview={effPrev2} onUpload={onUpload2} />
            </div>
            <Arrow />
            <div className="output-col">
                <OutputSlot label="Hybrid" histEntry={hybridEntry} chartEntry={null} containerRef={containerRef} />
                {hybridEntry?.result && (
                    <div style={{ display: 'flex', justifyContent: 'center', marginTop: 12 }}>
                        <button className="hist-btn save-btn-inline" onClick={() => saveBase64Image(hybridEntry.result, 'Hybrid_output.png')} title="Save Output">
                            <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2.2" strokeLinecap="round" strokeLinejoin="round">
                                <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4" />
                                <polyline points="7 10 12 15 17 10" /><line x1="12" y1="15" x2="12" y2="3" />
                            </svg>
                            Save Output
                        </button>
                    </div>
                )}
            </div>
        </div>
    );
}

/* ─── Root ─────────────────────────────────────────────────────────────────── */
function ImagePanels({
    operation,
    preview1, history1, chartResult1, onUpload1, onUndo1, onReset1,
    preview2, history2, chartResult2, onUpload2, onUndo2, onReset2,
    hybridResult
}) {
    return (
        <div className="image-panels">
            {operation === 'frequency' ? (
                <HybridLayout
                    preview1={preview1} onUpload1={onUpload1} history1={history1}
                    preview2={preview2} onUpload2={onUpload2} history2={history2}
                    hybridResult={hybridResult}
                />
            ) : (
                <>
                    <ImageRow label="Image 1" preview={preview1} history={history1} chartResult={chartResult1}
                        operation={operation} onUpload={onUpload1} onUndo={onUndo1} onReset={onReset1} />
                    <ImageRow label="Image 2" preview={preview2} history={history2} chartResult={chartResult2}
                        operation={operation} onUpload={onUpload2} onUndo={onUndo2} onReset={onReset2} />
                </>
            )}
        </div>
    );
}

export default ImagePanels;
