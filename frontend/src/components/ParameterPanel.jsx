import React from "react";

export default function ParameterPanel({ operation, onChange }) {
  const renderParams = () => {
    switch (operation) {
      case "salt_pepper_noise":
        return <input type="range" min="0" max="1" step="0.01" onChange={e => onChange({ratio: parseFloat(e.target.value)})} />;
      case "gaussian_noise":
        return <input type="number" min="0" onChange={e => onChange({sigma: parseFloat(e.target.value)})} />;
      case "median_filter":
        return <input type="number" min="1" step="2" onChange={e => onChange({kernel: parseInt(e.target.value)})} />;
      case "canny":
        return (
          <div className="flex gap-2">
            <input type="number" placeholder="Th1" onChange={e => onChange(prev => ({...prev, th1: parseInt(e.target.value)}))} />
            <input type="number" placeholder="Th2" onChange={e => onChange(prev => ({...prev, th2: parseInt(e.target.value)}))} />
          </div>
        );
      default: return null;
    }
  };
  return <div className="mb-4">{renderParams()}</div>;
}