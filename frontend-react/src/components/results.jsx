import React from 'react';

function Result({ result }) {
  if (!result) return <p>No result yet</p>;

  return (
    <div className="results">
      <h2>Processed Result</h2>
      <img src={`data:image/png;base64,${result}`} alt="Processed" />
    </div>
  );
}

export default Result;