import React from 'react';

// Slider component with value display and validation
const Slider = ({ label, name, value, onChange, min, max, step, unit = '', hint }) => {
  const displayValue = value !== undefined && value !== '' ? value : (min + max) / 2;

  return (
    <div className="slider-container">
      <div className="slider-header">
        <span className="slider-label">{label}</span>
        <span className="slider-value">{displayValue}{unit}</span>
      </div>
      <input
        type="range"
        name={name}
        min={min}
        max={max}
        step={step}
        value={displayValue}
        onChange={onChange}
      />
      <div className="slider-hints">
        <span>{min}{unit}</span>
        <span>{max}{unit}</span>
      </div>
      {hint && <div className="validation-hint info">ℹ️ {hint}</div>}
    </div>
  );
};

// Validation helper functions
const validateNoiseParams = (noiseType, params) => {
  const errors = [];

  if (noiseType === 'gaussian') {
    const sigma = parseFloat(params.sigma);
    if (sigma < 0 || sigma > 100) {
      errors.push('Sigma should be between 0 and 100');
    }
    const mean = parseFloat(params.mean);
    if (mean < -50 || mean > 50) {
      errors.push('Mean should be between -50 and 50');
    }
  } else if (noiseType === 'saltpepper') {
    const ratio = parseFloat(params.ratio);
    if (ratio < 0 || ratio > 1) {
      errors.push('Ratio must be between 0 and 1');
    }
  } else if (noiseType === 'uniform') {
    const alpha = parseFloat(params.alpha);
    if (alpha < 0 || alpha > 1) {
      errors.push('Alpha must be between 0 and 1');
    }
  }

  return errors;
};

function Controls({
  image1, setImage1,
  image2, setImage2,
  operation, setOperation,
  params, setParams,
  handleSubmit,
  loading,
  error
}) {

  const handleFileChange = (e, setImage) => {
    setImage(e.target.files[0]);
  };

  const handleParamChange = (e) => {
    setParams({
      ...params,
      [e.target.name]: e.target.value
    });
  };

  const handleSliderChange = (e) => {
    setParams({
      ...params,
      [e.target.name]: e.target.value
    });
  };

  const handleOperationChange = (e) => {
    setOperation(e.target.value);
    setParams({}); // Clear params when operation changes
  };

  // Get validation errors for current noise settings
  const noiseValidationErrors = operation === 'noise'
    ? validateNoiseParams(params.noiseType || 'gaussian', params)
    : [];

  return (
    <div className="controls">
      <h2>Controls</h2>
      
      <label>Image 1 (Required)</label>
      <input type="file" onChange={(e) => handleFileChange(e, setImage1)} />
      
      <label>Image 2 {operation === 'frequency' ? '(Required for frequency mixing)' : '(Optional)'}</label>
      <input type="file" onChange={(e) => handleFileChange(e, setImage2)} />
      
      <label>Operation</label>
      <select onChange={handleOperationChange} value={operation}>
        <option value="">Select operation</option>
        <option value="noise">Add Noise</option>
        <option value="filter">Noise Filtering</option>
        <option value="edge">Edge Detection</option>
        <option value="histogram">Histogram</option>
        <option value="equalize">Equalization/Normalization</option>
        <option value="grayscale">RGB to Grayscale</option>
        <option value="frequency">Hybrid Image (Frequency Mixing)</option>
      </select>

      {/* Noise Parameters with Sliders */}
      {operation === 'noise' && (
        <div className="control-section">
          <h3>Noise Settings</h3>

          <label>Noise Type</label>
          <select name="noiseType" onChange={handleParamChange} value={params.noiseType || 'gaussian'}>
            <option value="gaussian">Gaussian Noise</option>
            <option value="saltpepper">Salt and Pepper Noise</option>
            <option value="uniform">Uniform Noise</option>
          </select>

          {/* Gaussian Noise Parameters */}
          {(!params.noiseType || params.noiseType === 'gaussian') && (
            <div className="param-group">
              <h4>Gaussian Noise Parameters</h4>

              <Slider
                label="Mean"
                name="mean"
                value={params.mean || 0}
                onChange={handleSliderChange}
                min={-50}
                max={50}
                step={1}
                hint="Center of the noise distribution (typically 0)"
              />

              <Slider
                label="Sigma (Std Dev)"
                name="sigma"
                value={params.sigma || 25}
                onChange={handleSliderChange}
                min={0}
                max={100}
                step={1}
                hint="Higher values = more noise intensity"
              />
            </div>
          )}

          {/* Salt & Pepper Noise Parameters */}
          {params.noiseType === 'saltpepper' && (
            <div className="param-group">
              <h4>Salt and Pepper Parameters</h4>

              <Slider
                label="Noise Ratio"
                name="ratio"
                value={params.ratio || 0.05}
                onChange={handleSliderChange}
                min={0}
                max={0.5}
                step={0.01}
                hint="Probability of a pixel being affected (0-50%)"
              />

              <div className="validation-hint info">
                Recommended: 0.01 - 0.1 for realistic noise
              </div>
            </div>
          )}

          {/* Uniform Noise Parameters */}
          {params.noiseType === 'uniform' && (
            <div className="param-group">
              <h4>Uniform Noise Parameters</h4>

              <Slider
                label="Alpha (Intensity)"
                name="alpha"
                value={params.alpha || 0.1}
                onChange={handleSliderChange}
                min={0}
                max={1}
                step={0.05}
                hint="Noise intensity multiplier (0 = none, 1 = maximum)"
              />

              <div className="validation-hint info">
                Recommended: 0.05 - 0.3 for subtle effect
              </div>
            </div>
          )}

          {/* Validation Errors */}
          {noiseValidationErrors.length > 0 && (
            <div className="validation-hint error">
               {noiseValidationErrors.join(', ')}
            </div>
          )}
        </div>
      )}

      {/* Filter Parameters */}
      {operation === 'filter' && (
        <div className="control-section">
          <h3>Filter Settings</h3>

          <label>Filter Type</label>
          <select name="filterType" onChange={handleParamChange} value={params.filterType || 'average'}>
            <option value="average">Average Filter</option>
            <option value="median">Median Filter</option>
            <option value="gaussian">Gaussian Filter</option>
          </select>

          <div className="param-group">
            <h4>Filter Parameters</h4>

            <Slider
              label="Kernel Size"
              name="kernelSize"
              value={params.kernelSize || 3}
              onChange={(e) => {
                // Ensure kernel size is odd
                let val = parseInt(e.target.value);
                if (val % 2 === 0) val = val + 1;
                setParams({ ...params, kernelSize: val });
              }}
              min={3}
              max={15}
              step={2}
              hint="Must be odd number (3, 5, 7, etc.)"
            />
          </div>
        </div>
      )}

      {/* Edge Detection Parameters */}
      {operation === 'edge' && (
        <div className="control-section">
          <h3>Edge Detection Settings</h3>

          <label>Edge Detection Method</label>
          <select name="edgeType" onChange={handleParamChange} value={params.edgeType || 'sobel'}>
            <option value="sobel">Sobel</option>
            <option value="prewitt">Prewitt</option>
            <option value="roberts">Roberts</option>
            <option value="canny">Canny</option>
          </select>

          <div className="param-group">
            <h4>Edge Parameters</h4>
            
            <label>Threshold 1 (Low)</label>
            <input 
              type="number" 
              name="threshold" 
              min="0" 
              max="255" 
              value={params.threshold || 50}
              onChange={handleParamChange} 
            />

            <label>Threshold 2 (High)</label>
            <input 
              type="number" 
              name="threshold2" 
              min="0" 
              max="255" 
              value={params.threshold2 || 150}
              onChange={handleParamChange} 
            />
          </div>
        </div>
      )}

      {/* Frequency/Hybrid Parameters */}
      {operation === 'frequency' && (
        <div className="control-section">
          <h3>Hybrid Image Settings</h3>
          
          <div className="filter-section">
            <h4>Image 1 Filter Settings</h4>
            <select name="filterType1" onChange={handleParamChange} value={params.filterType1 || 'lowpass'}>
              <option value="none">No Filter</option>
              <option value="lowpass">Low Pass Filter</option>
              <option value="highpass">High Pass Filter</option>
            </select>
          </div>

          <div className="filter-section">
            <h4>Image 2 Filter Settings</h4>
            <select name="filterType2" onChange={handleParamChange} value={params.filterType2 || 'highpass'}>
              <option value="none">No Filter</option>
              <option value="lowpass">Low Pass Filter</option>
              <option value="highpass">High Pass Filter</option>
            </select>
          </div>

        </div>
      )}

      {/* Submit Button */}
      <button
        onClick={handleSubmit}
        disabled={loading || !image1 || !operation || (operation === 'frequency' && !image2)}
      >
        {loading ? (
          <span className="button-content">
            <span className="loading-spinner"></span>
            <span>Processing...</span>
          </span>
        ) : (
          <span className="button-content">
            <span>Process Image</span>
          </span>
        )}
      </button>

      {/* Error Display */}
      {error && <div className="error-message">{error}</div>}
    </div>
  );
}

export default Controls;