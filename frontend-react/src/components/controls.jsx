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
      <h2>Image Controls</h2>

      {/* Image Upload Section */}
      <div className="control-section">
        <h3>📁 Image Upload</h3>
        <label>
          Primary Image <span className="badge required">Required</span>
        </label>
        <div className="file-input-wrapper">
          <input
            type="file"
            accept="image/*"
            onChange={(e) => handleFileChange(e, setImage1)}
          />
        </div>
        {image1 && (
          <div className="validation-hint success">✓ {image1.name} selected</div>
        )}

        <label>
          Secondary Image <span className="badge optional">Optional</span>
        </label>
        <div className="file-input-wrapper">
          <input
            type="file"
            accept="image/*"
            onChange={(e) => handleFileChange(e, setImage2)}
          />
        </div>
        {image2 && (
          <div className="validation-hint success">✓ {image2.name} selected</div>
        )}
      </div>

      {/* Operation Selection */}
      <div className="control-section">
        <h3>🔧 Operation</h3>
        <select onChange={handleOperationChange} value={operation}>
          <option value="">-- Select an operation --</option>
          <option value="noise">🎲 Add Noise</option>
          <option value="filter">🔍 Noise Filtering</option>
          <option value="edge">📐 Edge Detection</option>
          <option value="histogram">📊 Histogram</option>
          <option value="equalize">⚖️ Equalization</option>
          <option value="grayscale">🎨 RGB → Grayscale</option>
          <option value="frequency">🌊 Frequency Mixing</option>
        </select>
      </div>

      {/* Noise Parameters with Sliders */}
      {operation === 'noise' && (
        <div className="control-section">
          <h3>🎲 Noise Settings</h3>

          <label>Noise Type</label>
          <select name="noiseType" onChange={handleParamChange} value={params.noiseType || 'gaussian'}>
            <option value="gaussian">Gaussian Noise</option>
            <option value="saltpepper">Salt & Pepper Noise</option>
            <option value="uniform">Uniform Noise</option>
          </select>

          {/* Gaussian Noise Parameters */}
          {(!params.noiseType || params.noiseType === 'gaussian') && (
            <div className="param-group">
              <h4>🔔 Gaussian Noise Parameters</h4>

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
              <h4>🧂 Salt & Pepper Parameters</h4>

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
                ℹ️ Recommended: 0.01 - 0.1 for realistic noise
              </div>
            </div>
          )}

          {/* Uniform Noise Parameters */}
          {params.noiseType === 'uniform' && (
            <div className="param-group">
              <h4>📊 Uniform Noise Parameters</h4>

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
                ℹ️ Recommended: 0.05 - 0.3 for subtle effect
              </div>
            </div>
          )}

          {/* Validation Errors */}
          {noiseValidationErrors.length > 0 && (
            <div className="validation-hint error">
              ⚠️ {noiseValidationErrors.join(', ')}
            </div>
          )}
        </div>
      )}

      {/* Filter Parameters */}
      {operation === 'filter' && (
        <div className="control-section">
          <h3>🔍 Filter Settings</h3>

          <label>Filter Type</label>
          <select name="filterType" onChange={handleParamChange} value={params.filterType || 'average'}>
            <option value="average">Average Filter</option>
            <option value="median">Median Filter</option>
            <option value="gaussian">Gaussian Filter</option>
          </select>

          <div className="param-group">
            <h4>⚙️ Filter Parameters</h4>

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
          <h3>📐 Edge Detection Settings</h3>

          <label>Edge Detection Method</label>
          <select name="edgeType" onChange={handleParamChange} value={params.edgeType || 'sobel'}>
            <option value="sobel">Sobel</option>
            <option value="prewitt">Prewitt</option>
            <option value="roberts">Roberts</option>
            <option value="canny">Canny</option>
          </select>

          {/* Canny Edge Detection Parameters */}
          {params.edgeType === 'canny' && (
            <div className="param-group">
              <h4>🎯 Canny Thresholds</h4>

              <Slider
                label="Lower Threshold"
                name="threshold"
                value={params.threshold || 50}
                onChange={handleSliderChange}
                min={0}
                max={255}
                step={5}
                hint="Edges below this are discarded"
              />

              <Slider
                label="Upper Threshold"
                name="threshold2"
                value={params.threshold2 || 150}
                onChange={handleSliderChange}
                min={0}
                max={255}
                step={5}
                hint="Edges above this are strong edges"
              />

              {parseInt(params.threshold) >= parseInt(params.threshold2) && (
                <div className="validation-hint warning">
                  ⚠️ Lower threshold should be less than upper threshold
                </div>
              )}
            </div>
          )}
        </div>
      )}

      {/* Frequency Filter Parameters */}
      {operation === 'frequency' && (
        <div className="control-section">
          <h3>🌊 Frequency Filter Settings</h3>

          <label>Filter Type</label>
          <select name="freqType" onChange={handleParamChange} value={params.freqType || 'lpf'}>
            <option value="lpf">Low Pass Filter (LPF)</option>
            <option value="hpf">High Pass Filter (HPF)</option>
          </select>

          <div className="param-group">
            <h4>📶 Cutoff Frequency</h4>

            <Slider
              label="Cutoff"
              name="cutoff"
              value={params.cutoff || 30}
              onChange={handleSliderChange}
              min={1}
              max={100}
              step={1}
              hint={params.freqType === 'hpf'
                ? "Higher = more details preserved"
                : "Lower = more smoothing"}
            />
          </div>
        </div>
      )}

      {/* Submit Button */}
      <button
        onClick={handleSubmit}
        disabled={loading || !image1 || !operation || noiseValidationErrors.length > 0}
      >
        {loading ? (
          <span className="button-content">
            <span className="loading-spinner"></span>
            <span>Processing...</span>
          </span>
        ) : (
          <span className="button-content">
            <span>🚀</span>
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

