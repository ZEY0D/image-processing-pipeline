import React, { useState, useCallback } from 'react';
import axios from 'axios';
import Controls from './components/controls';
import ImagePanels from './components/ImagePanels';
import './index.css';

function App() {
  // Image files
  const [image1, setImage1] = useState(null);
  const [image2, setImage2] = useState(null);
  // Preview data-URLs shown immediately after upload
  const [preview1, setPreview1] = useState(null);
  const [preview2, setPreview2] = useState(null);
  // Processed results per image
  const [result1, setResult1] = useState(null);
  const [result2, setResult2] = useState(null);
  // Histogram data per image (only for equalize)
  const [histBefore1, setHistBefore1] = useState(null);
  const [histAfter1, setHistAfter1] = useState(null);
  const [histBefore2, setHistBefore2] = useState(null);
  const [histAfter2, setHistAfter2] = useState(null);
  // RGB histogram data per image (for histogram operation)
  const [rgbHist1, setRgbHist1] = useState(null);
  const [rgbHist2, setRgbHist2] = useState(null);
  // CDF data per image
  const [cdfData1, setCdfData1] = useState(null);
  const [cdfData2, setCdfData2] = useState(null);

  // Operation + params
  const [operation, setOperation] = useState('');
  const [params, setParams] = useState({});
  // Which image the controls apply to
  const [activeImage, setActiveImage] = useState('1');

  const [loading, setLoading] = useState(false);
  const [error, setError] = useState('');

  // ── Upload handlers ───────────────────────────────
  const handleUpload1 = useCallback((file) => {
    setImage1(file);
    setPreview1(URL.createObjectURL(file));
  }, []);

  const handleUpload2 = useCallback((file) => {
    setImage2(file);
    setPreview2(URL.createObjectURL(file));
  }, []);

  // ── Base64 helper ─────────────────────────────────
  const fileToBase64 = (file) =>
    new Promise((resolve, reject) => {
      const reader = new FileReader();
      reader.onload = () => resolve(reader.result.split(',')[1]);
      reader.onerror = reject;
      reader.readAsDataURL(file);
    });

  // ── Operation resolver ────────────────────────────
  const resolveOperation = (uiOperation, p) => {
    switch (uiOperation) {
      case 'noise':
        if (p.noiseType === 'saltpepper') return 'salt_pepper_noise';
        if (p.noiseType === 'gaussian') return 'gaussian_noise';
        if (p.noiseType === 'uniform') return 'uniform_noise';
        return 'gaussian_noise';

      case 'filter':
        if (p.filterType === 'average') return 'average_filter';
        if (p.filterType === 'gaussian') return 'gaussian_filter';
        return 'median_filter';

      case 'edge':
        if (p.edgeType === 'prewitt') return 'prewitt';
        if (p.edgeType === 'roberts') return 'roberts';
        if (p.edgeType === 'canny') return 'canny';
        return 'sobel';

      case 'histogram': return 'histogram';
      case 'cdf': return 'cdf';
      case 'normalize': return 'normalize';
      case 'equalize': return 'equalize';
      case 'grayscale': return 'grayscale';
      case 'frequency': return 'hybrid_image';

      default: return uiOperation;
    }
  };

  // ── Submit ────────────────────────────────────────
  const handleSubmit = async () => {
    const activeFile = activeImage === '1' ? image1 : image2;

    if (!activeFile || !operation) {
      setError(`Upload Image ${activeImage} and select an operation first`);
      return;
    }

    // Hybrid image needs both
    if (operation === 'frequency' && (!image1 || !image2)) {
      setError('Hybrid Image requires both Image 1 and Image 2 to be uploaded');
      return;
    }

    setLoading(true);
    setError('');

    try {
      const imageBase64 = await fileToBase64(activeFile);
      let image2Base64 = null;
      if (operation === 'frequency' && image2) {
        image2Base64 = await fileToBase64(activeImage === '1' ? image2 : image1);
      }

      const backendOperation = resolveOperation(operation, params);

      const payload = {
        image: imageBase64,
        image2: image2Base64,
        operation: backendOperation,
        ratio: parseFloat(params.ratio) || 0.05,
        sigma: parseFloat(params.sigma) || 25,
        mean: parseFloat(params.mean) || 0,
        alpha: parseFloat(params.alpha) || 0.1,
        kernel: parseInt(params.kernelSize) || 3,
        th1: parseInt(params.threshold) || 50,
        th2: parseInt(params.threshold2) || 150,
        filterType1: params.filterType1 || 'lowpass',
        filterType2: params.filterType2 || 'highpass',
      };

      // Drop null/undefined
      Object.keys(payload).forEach(k => payload[k] == null && delete payload[k]);

      const res = await axios.post('/process', payload, {
        headers: { 'Content-Type': 'application/json' },
      });

      // Store result
      if (activeImage === '1') {
        setResult1(res.data.result || null);
        setHistBefore1(res.data.histogram_before || null);
        setHistAfter1(res.data.histogram_after || null);
        setRgbHist1(res.data.histogram_r ? {
          r: res.data.histogram_r, g: res.data.histogram_g, b: res.data.histogram_b,
        } : null);
        setCdfData1(res.data.cdf_r ? {
          r: res.data.cdf_r, g: res.data.cdf_g, b: res.data.cdf_b,
        } : null);
      } else {
        setResult2(res.data.result || null);
        setHistBefore2(res.data.histogram_before || null);
        setHistAfter2(res.data.histogram_after || null);
        setRgbHist2(res.data.histogram_r ? {
          r: res.data.histogram_r, g: res.data.histogram_g, b: res.data.histogram_b,
        } : null);
        setCdfData2(res.data.cdf_r ? {
          r: res.data.cdf_r, g: res.data.cdf_g, b: res.data.cdf_b,
        } : null);
      }

    } catch (err) {
      const msg = err?.response?.data?.error;
      setError(msg ? `Backend error: ${msg}` : `Request failed: ${err.message}`);
    }
    setLoading(false);
  };

  return (
    <div className="app-shell">
      <header className="topbar">
        <div className="topbar-logo">
          <svg viewBox="0 0 24 24">
            <rect x="3" y="3" width="7" height="7" />
            <rect x="14" y="3" width="7" height="7" />
            <rect x="3" y="14" width="7" height="7" />
            <rect x="14" y="14" width="7" height="7" />
          </svg>
        </div>
        <span className="topbar-title">Image Processing Pipeline</span>
        <span className="topbar-subtitle">— OpenCV C++ Backend</span>
      </header>

      <div className="container">
        <Controls
          activeImage={activeImage} setActiveImage={setActiveImage}
          operation={operation} setOperation={setOperation}
          params={params} setParams={setParams}
          handleSubmit={handleSubmit}
          loading={loading}
          error={error}
          image1={image1}
          image2={image2}
        />
        <ImagePanels
          preview1={preview1} result1={result1} onUpload1={handleUpload1}
          histBefore1={histBefore1} histAfter1={histAfter1} rgbHist1={rgbHist1} cdfData1={cdfData1}
          preview2={preview2} result2={result2} onUpload2={handleUpload2}
          histBefore2={histBefore2} histAfter2={histAfter2} rgbHist2={rgbHist2} cdfData2={cdfData2}
        />
      </div>
    </div>
  );
}

export default App;