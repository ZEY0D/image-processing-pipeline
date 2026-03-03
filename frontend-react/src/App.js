import React, { useState, useCallback } from 'react';
import axios from 'axios';
import Controls from './components/controls';
import ImagePanels from './components/ImagePanels';
import './index.css';

const OP_LABELS = {
  noise: 'Noise', filter: 'Filter', edge: 'Edge',
  histogram: 'Histogram', cdf: 'CDF',
  normalize: 'Normalize', equalize: 'Equalize',
  grayscale: 'Grayscale', 
  frequency_lpf: 'Low-Pass Filter (Freq)',
  frequency_hpf: 'High-Pass Filter (Freq)',
  frequency_lpf_spectrum: 'Low-Pass Spectrum',
  frequency_hpf_spectrum: 'High-Pass Spectrum',
  frequency: 'Hybrid',
};

// These operations produce charts, not editable results — they don't go into history
const ANALYSIS_OPS = new Set(['histogram', 'cdf']);

function App() {
  const [image1, setImage1] = useState(null);
  const [image2, setImage2] = useState(null);
  const [preview1, setPreview1] = useState(null);
  const [preview2, setPreview2] = useState(null);

  // Edit history per image (image-producing operations only)
  const [history1, setHistory1] = useState([]);
  const [history2, setHistory2] = useState([]);

  // Separate chart results (histogram / CDF) — not part of history
  const [chartResult1, setChartResult1] = useState(null); // { rgbHist, cdfData, opLabel }
  const [chartResult2, setChartResult2] = useState(null);

  // Dedicated state for Hybrid operation output so it doesn't pollute history
  const [hybridResult, setHybridResult] = useState(null);

  const [operation, setOperation] = useState('');
  const [params, setParams] = useState({});
  const [activeImage, setActiveImage] = useState('1');
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState('');

  const handleUpload1 = useCallback((file) => {
    setImage1(file);
    setPreview1(URL.createObjectURL(file));
    setHistory1([]);
    setChartResult1(null);
  }, []);

  const handleUpload2 = useCallback((file) => {
    setImage2(file);
    setPreview2(URL.createObjectURL(file));
    setHistory2([]);
    setChartResult2(null);
  }, []);

  const handleUndo1 = () => setHistory1(prev => prev.slice(0, -1));
  const handleUndo2 = () => setHistory2(prev => prev.slice(0, -1));
  const handleReset1 = () => { setHistory1([]); setChartResult1(null); };
  const handleReset2 = () => { setHistory2([]); setChartResult2(null); };

  const fileToBase64 = (file) =>
    new Promise((resolve, reject) => {
      const reader = new FileReader();
      reader.onload = () => resolve(reader.result.split(',')[1]);
      reader.onerror = reject;
      reader.readAsDataURL(file);
    });

  const resolveOperation = (uiOp, p) => {
    switch (uiOp) {
      case 'noise':
        if (p.noiseType === 'saltpepper') return 'salt_pepper_noise';
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
      case 'frequency_lpf': return 'frequency_lpf';
      case 'frequency_hpf': return 'frequency_hpf';
      case 'frequency_lpf_spectrum': return 'frequency_lpf_spectrum';
      case 'frequency_hpf_spectrum': return 'frequency_hpf_spectrum';
      case 'frequency': return 'hybrid_image';
      default: return uiOp;
    }
  };

  const handleSubmit = async () => {
    const activeFile = activeImage === '1' ? image1 : image2;
    const activeHistory = activeImage === '1' ? history1 : history2;

    if (!activeFile || !operation) {
      setError(`Upload Image ${activeImage} and select an operation first`);
      return;
    }
    if (operation === 'frequency' && (!image1 || !image2)) {
      setError('Hybrid Image requires both images to be uploaded');
      return;
    }

    setLoading(true);
    setError('');

    try {
      let imageBase64;
      if (operation === 'frequency') {
        imageBase64 = await fileToBase64(activeFile);
      } else {
        const lastResultEntry = [...activeHistory].reverse().find(h => h.result);
        imageBase64 = lastResultEntry
          ? lastResultEntry.result
          : await fileToBase64(activeFile);
      }

      let image2Base64 = null;
      if (operation === 'frequency') {
        image2Base64 = await fileToBase64(activeImage === '1' ? image2 : image1);
      }

      const backendOperation = resolveOperation(operation, params);

      const payload = {
        image: imageBase64, image2: image2Base64,
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
      Object.keys(payload).forEach(k => payload[k] == null && delete payload[k]);

      const res = await axios.post('/process', payload, {
        headers: { 'Content-Type': 'application/json' },
      });

      const opLabel = OP_LABELS[operation] || operation;

      if (operation === 'frequency') {
        // Hybrid op — store completely separately from history
        setHybridResult(res.data.result || null);
      } else if (ANALYSIS_OPS.has(operation)) {
        // Chart-only ops — store separately, don't touch history
        const chartEntry = {
          opLabel,
          rgbHist: res.data.histogram_r ? {
            r: res.data.histogram_r, g: res.data.histogram_g, b: res.data.histogram_b,
          } : null,
          cdfData: res.data.cdf_r ? {
            r: res.data.cdf_r, g: res.data.cdf_g, b: res.data.cdf_b,
          } : null,
        };
        if (activeImage === '1') setChartResult1(chartEntry);
        else setChartResult2(chartEntry);
      } else {
        // Image-producing ops (filters, edge, equalize etc) — push to history, clear chart result for this slot
        const entry = {
          opLabel,
          result: res.data.result || null,
          histBefore: res.data.histogram_before || null,
          histAfter: res.data.histogram_after || null,
        };
        if (activeImage === '1') {
          setHistory1(prev => [...prev, entry]);
          setChartResult1(null);
        } else {
          setHistory2(prev => [...prev, entry]);
          setChartResult2(null);
        }
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
        <svg width="26" height="26" viewBox="0 0 24 24" fill="none"
          stroke="#111" strokeWidth="1.8" strokeLinecap="round" strokeLinejoin="round">
          <rect x="3" y="3" width="20" height="20" rx="2" />
          <circle cx="8.5" cy="8.5" r="1.5" />
          <polyline points="21 15 16 10 5 21" />
        </svg>
        <span className="topbar-title">Image Processing Website</span>
      </header>

      <div className="container">
        <Controls
          activeImage={activeImage} setActiveImage={setActiveImage}
          operation={operation} setOperation={setOperation}
          params={params} setParams={setParams}
          handleSubmit={handleSubmit}
          loading={loading} error={error}
          image1={image1} image2={image2}
        />
        <ImagePanels
          operation={operation}
          preview1={preview1} history1={history1} chartResult1={chartResult1}
          onUpload1={handleUpload1} onUndo1={handleUndo1} onReset1={handleReset1}
          preview2={preview2} history2={history2} chartResult2={chartResult2}
          onUpload2={handleUpload2} onUndo2={handleUndo2} onReset2={handleReset2}
          hybridResult={hybridResult}
        />
      </div>
    </div>
  );
}

export default App;