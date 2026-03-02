import React, { useState } from 'react';
import axios from 'axios';
import Controls from './components/controls';
import Result from './components/results';
import './index.css';

function App() {
  const [image1, setImage1] = useState(null);
  const [image2, setImage2] = useState(null);
  const [operation, setOperation] = useState('');
  const [params, setParams] = useState({});
  const [result, setResult] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState('');

  // Convert a File object to a base64 string (without the data:...;base64, prefix)
  const fileToBase64 = (file) =>
    new Promise((resolve, reject) => {
      const reader = new FileReader();
      reader.onload = () => resolve(reader.result.split(',')[1]);
      reader.onerror = reject;
      reader.readAsDataURL(file);
    });

  // Map the Controls UI operation values → backend operation strings
  const resolveOperation = (uiOperation, p) => {
    switch (uiOperation) {
      case 'noise':
        if (p.noiseType === 'saltpepper') return 'salt_pepper_noise';
        if (p.noiseType === 'gaussian') return 'gaussian_noise';
        if (p.noiseType === 'uniform') return 'uniform_noise';
        return 'gaussian_noise'; // default

      case 'filter':
        if (p.filterType === 'average') return 'average_filter';
        if (p.filterType === 'gaussian') return 'gaussian_filter';
        return 'median_filter'; // default (median)

      case 'edge':
        if (p.edgeType === 'prewitt') return 'prewitt';
        if (p.edgeType === 'roberts') return 'roberts';
        if (p.edgeType === 'canny') return 'canny';
        return 'sobel'; // default

      case 'histogram': return 'histogram';
      case 'equalize': return 'equalize';
      case 'grayscale': return 'grayscale';
      case 'frequency': return 'hybrid_image';

      default: return uiOperation;
    }
  };

  const handleSubmit = async () => {
    if (!image1 || !operation) {
      setError('Image and operation are required');
      return;
    }
    
    // For frequency operation, image2 is required
    if (operation === 'frequency' && !image2) {
      setError('Second image is required for frequency mixing');
      return;
    }
    
    setLoading(true);
    setError('');

    try {
      const imageBase64 = await fileToBase64(image1);
      let image2Base64 = null;
      if (image2) {
        image2Base64 = await fileToBase64(image2);
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
        // For frequency operations - only filter types, no cutoff
        filterType1: params.filterType1 || 'lowpass',
        filterType2: params.filterType2 || 'highpass',
      };

      // Remove any undefined values
      Object.keys(payload).forEach(key => 
        payload[key] === undefined && delete payload[key]
      );

      console.log('Sending operation:', backendOperation);
      console.log('Payload (without image):', { ...payload, image: '[base64 truncated]' });

      // Use proxy - setupProxy.js forwards /process to http://127.0.0.1:18080/process
      const res = await axios.post('/process', payload, {
        headers: { 'Content-Type': 'application/json' },
      });
      console.log('Response:', res.data);
      setResult(res.data.result);
    } catch (err) {
      // Show the real error message returned by the backend
      console.error('Error response:', err?.response?.data);
      const backendMsg = err?.response?.data?.error;
      setError(backendMsg
        ? `Backend error: ${backendMsg}`
        : `Request failed: ${err.message}`
      );
    }
    setLoading(false);
  };

  return (
    <div className="container">
      <Controls
        image1={image1} setImage1={setImage1}
        image2={image2} setImage2={setImage2}
        operation={operation} setOperation={setOperation}
        params={params} setParams={setParams}
        handleSubmit={handleSubmit}
        loading={loading}
        error={error}
      />
      <Result result={result} />
    </div>
  );
}

export default App;