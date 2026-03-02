import { useState } from "react";
import api from "../api";
import ImageUploader from "../components/ImageUploader";
import OperationSelector from "../components/OperationSelector";
import ParameterPanel from "../components/ParameterPanel";
import ImageDisplay from "../components/ImageDisplay";

export default function Dashboard() {
  const [image, setImage] = useState(null);
  const [operation, setOperation] = useState("");
  const [params, setParams] = useState({});
  const [result, setResult] = useState(null);
  const [loading, setLoading] = useState(false);

  const handleProcess = async () => {
    if (!image || !operation) return;
    setLoading(true);
    try {
      const response = await api.post("/process", { operation, image, ...params });
      setResult(response.data.result);
    } catch (err) {
      console.error(err);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="flex h-screen">
      <div className="w-1/4 p-4 bg-gray-100">
        <ImageUploader onChange={setImage} />
        <OperationSelector value={operation} onChange={setOperation} />
        <ParameterPanel operation={operation} onChange={setParams} />
        <button onClick={handleProcess} disabled={!operation || !image || loading} className="mt-4 w-full bg-blue-500 text-white py-2 rounded disabled:opacity-50">
          {loading ? "Processing..." : "Run"}
        </button>
      </div>
      <div className="flex-1 p-4 overflow-auto">
        {image && <ImageDisplay title="Original" src={image} />}
        {result && <ImageDisplay title="Result" src={result} />}
      </div>
    </div>
  );
}