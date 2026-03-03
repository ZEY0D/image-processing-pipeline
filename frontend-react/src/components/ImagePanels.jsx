import React, { useRef } from 'react';
import Histogram from './Histogram';
import RGBHistogram from './RGBHistogram';
import CDFChart from './CDFChart';

function ImageSlot({ label, preview, result, onUpload, histBefore, histAfter, rgbHist, cdfData }) {
    const fileRef = useRef();

    const handleDrop = (e) => {
        e.preventDefault();
        const file = e.dataTransfer.files[0];
        if (file) onUpload(file);
    };

    return (
        <div className="image-row">
            <div className="image-row-label">{label}</div>

            <div className="image-row-panels">
                {/* LEFT — Upload / Preview */}
                <div
                    className={`image-slot${preview ? ' has-image' : ''}`}
                    onClick={() => fileRef.current.click()}
                    onDragOver={(e) => e.preventDefault()}
                    onDrop={handleDrop}
                >
                    <input ref={fileRef} type="file" accept="image/*" style={{ display: 'none' }}
                        onChange={(e) => e.target.files[0] && onUpload(e.target.files[0])} />
                    {preview ? (
                        <img src={preview} alt={`${label} input`} className="slot-img" />
                    ) : (
                        <div className="slot-placeholder">
                            <svg width="32" height="32" viewBox="0 0 24 24" fill="none"
                                stroke="currentColor" strokeWidth="1.5" strokeLinecap="round" strokeLinejoin="round">
                                <rect x="3" y="3" width="18" height="18" rx="2" />
                                <circle cx="8.5" cy="8.5" r="1.5" />
                                <polyline points="21 15 16 10 5 21" />
                            </svg>
                            <span>Click or drop to upload</span>
                        </div>
                    )}
                    {preview && (
                        <div className="slot-overlay">
                            <svg width="18" height="18" viewBox="0 0 24 24" fill="none"
                                stroke="white" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
                                <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4" />
                                <polyline points="17 8 12 3 7 8" />
                                <line x1="12" y1="3" x2="12" y2="15" />
                            </svg>
                            <span>Replace</span>
                        </div>
                    )}
                </div>

                {/* Arrow */}
                <div className="slot-arrow">
                    <svg width="18" height="18" viewBox="0 0 24 24" fill="none"
                        stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
                        <line x1="5" y1="12" x2="19" y2="12" />
                        <polyline points="12 5 19 12 12 19" />
                    </svg>
                </div>

                {/* RIGHT — Output slot */}
                <div className={`image-slot output-slot${(result || rgbHist || cdfData) ? ' has-image' : ''}`}>
                    {result ? (
                        <img src={`data:image/png;base64,${result}`} alt={`${label} output`} className="slot-img" />
                    ) : rgbHist ? (
                        <div className="rgb-hist-output">
                            <RGBHistogram histR={rgbHist.r} histG={rgbHist.g} histB={rgbHist.b} />
                        </div>
                    ) : cdfData ? (
                        <div className="rgb-hist-output">
                            <CDFChart cdfR={cdfData.r} cdfG={cdfData.g} cdfB={cdfData.b} />
                        </div>
                    ) : (
                        <div className="slot-placeholder output">
                            <svg width="32" height="32" viewBox="0 0 24 24" fill="none"
                                stroke="currentColor" strokeWidth="1.5" strokeLinecap="round" strokeLinejoin="round">
                                <polyline points="22 12 18 12 15 21 9 3 6 12 2 12" />
                            </svg>
                            <span>Output will appear here</span>
                        </div>
                    )}
                </div>
            </div>

            {/* Grayscale before/after histograms (equalize only) */}
            {(histBefore || histAfter) && (
                <div className="histogram-row">
                    <div className="histogram-side">
                        {histBefore && <Histogram data={histBefore} title="BEFORE" />}
                    </div>
                    <div className="histogram-arrow-spacer" />
                    <div className="histogram-side">
                        {histAfter && <Histogram data={histAfter} title="AFTER" />}
                    </div>
                </div>
            )}
        </div>
    );
}

function ImagePanels({ image1, preview1, result1, onUpload1, histBefore1, histAfter1, rgbHist1, cdfData1,
    image2, preview2, result2, onUpload2, histBefore2, histAfter2, rgbHist2, cdfData2 }) {
    return (
        <div className="image-panels">
            <ImageSlot label="Image 1" preview={preview1} result={result1}
                onUpload={onUpload1} histBefore={histBefore1} histAfter={histAfter1}
                rgbHist={rgbHist1} cdfData={cdfData1} />
            <ImageSlot label="Image 2" preview={preview2} result={result2}
                onUpload={onUpload2} histBefore={histBefore2} histAfter={histAfter2}
                rgbHist={rgbHist2} cdfData={cdfData2} />
        </div>
    );
}

export default ImagePanels;
