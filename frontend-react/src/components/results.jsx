import React from 'react';

function Result({ result }) {
  return (
    <div className="results">
      <h2>Processed Result</h2>
      <div className="result-image-container">
        {result ? (
          <img src={`data:image/png;base64,${result}`} alt="Processed" />
        ) : (
          <div className="empty-state">
            <div className="empty-state-icon">🖼️</div>
            <div className="empty-state-text">No result yet</div>
            <div className="empty-state-subtext">
              Upload an image and select an operation to get started
            </div>
          </div>
        )}
      </div>
    </div>
  );
}

export default Result;