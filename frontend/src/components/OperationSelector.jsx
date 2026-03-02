import React from "react";

const operations = [
  { value: "salt_pepper_noise", label: "Salt & Pepper Noise" },
  { value: "gaussian_noise", label: "Gaussian Noise" },
  { value: "median_filter", label: "Median Filter" },
  { value: "sobel", label: "Sobel Edge" },
  { value: "canny", label: "Canny Edge" },
];

export default function OperationSelector({ value, onChange }) {
  return (
    <div className="mb-4">
      <label className="block mb-2 font-bold">Select Operation</label>
      <select value={value} onChange={(e) => onChange(e.target.value)} className="w-full p-2 border rounded">
        <option value="">-- Choose --</option>
        {operations.map(op => <option key={op.value} value={op.value}>{op.label}</option>)}
      </select>
    </div>
  );
}