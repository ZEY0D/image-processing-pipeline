import React from "react";

export default function ImageDisplay({ title, src }) {
  return (
    <div className="mb-4">
      <h3 className="font-bold mb-2">{title}</h3>
      <img src={src} alt={title} className="max-w-full border rounded" />
    </div>
  );
}