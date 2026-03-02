import React from 'react';

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

  return (
    <div className="controls">
      <h2>Controls</h2>
      
      <label>Image 1 (Required)</label>
      <input type="file" onChange={(e) => handleFileChange(e, setImage1)} />
      
      <label>Image 2 (Optional, for frequency mixing)</label>
      <input type="file" onChange={(e) => handleFileChange(e, setImage2)} />
      
      <label>Operation</label>
      <select onChange={(e) => setOperation(e.target.value)} value={operation}>
        <option value="">Select operation</option>
        <option value="noise">Add Noise</option>
        <option value="filter">Noise Filtering</option>
        <option value="edge">Edge Detection</option>
        <option value="histogram">Histogram</option>
        <option value="equalize">Equalization/Normalization</option>
        <option value="grayscale">RGB → Grayscale</option>
        <option value="frequency">Frequency Mixing</option>
      </select>

      {/* Dynamic parameters example */}
      {operation === 'noise' && (
        <>
          <label>Noise Type</label>
          <select name="noiseType" onChange={handleParamChange}>
            <option value="uniform">Uniform</option>
            <option value="gaussian">Gaussian</option>
            <option value="saltpepper">Salt & Pepper</option>
          </select>

          <label>Ratio (for Salt & Pepper)</label>
          <input type="number" name="ratio" min="0" max="1" step="0.01" onChange={handleParamChange} />
        </>
      )}

      {operation === 'filter' && (
        <>
          <label>Filter Type</label>
          <select name="filterType" onChange={handleParamChange}>
            <option value="average">Average</option>
            <option value="median">Median</option>
            <option value="gaussian">Gaussian</option>
          </select>

          <label>Kernel Size</label>
          <input type="number" name="kernelSize" min="1" step="2" onChange={handleParamChange} />
        </>
      )}

      {operation === 'edge' && (
        <>
          <label>Edge Type</label>
          <select name="edgeType" onChange={handleParamChange}>
            <option value="sobel">Sobel</option>
            <option value="prewitt">Prewitt</option>
            <option value="roberts">Roberts</option>
            <option value="canny">Canny</option>
          </select>

          <label>Threshold (Canny only)</label>
          <input type="number" name="threshold" min="0" max="255" onChange={handleParamChange} />
        </>
      )}

      <button
        onClick={handleSubmit}
        disabled={loading || !image1 || !operation}
      >
        {loading ? 'Processing...' : 'Submit'}
      </button>

      {error && <p style={{color:'red'}}>{error}</p>}
    </div>
  );
}

export default Controls;