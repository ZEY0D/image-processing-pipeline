import React from "react";

export default function ImageUploader({ onChange }) {
  const handleFile = (e) => {
    const file = e.target.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onloadend = () => onChange(reader.result);
    reader.readAsDataURL(file);
  };

  return (
    <div className="mb-4">
      <label className="block mb-2 font-bold">Upload Image</label>
      <input type="file" accept="image/*" onChange={handleFile} />
    </div>
  );
}