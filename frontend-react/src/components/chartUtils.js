/**
 * chartUtils.js — Shared Canvas drawing helpers for histogram/CDF charts.
 * Draws axes, grid, labels, and title. Returns the inner chart area dimensions.
 */

/**
 * Draw the full chart frame: clear, white fill, grid, border, y-axis (numbers +
 * rotated label), x-axis (numbers + label), and optional title.
 *
 * @param {CanvasRenderingContext2D} ctx
 * @param {number} W   Canvas pixel width
 * @param {number} H   Canvas pixel height
 * @param {{ left, right, top, bottom }} pads  Padding on each side
 * @param {object} opts
 *   @param {number}   opts.maxVal     Peak y value (for tick labels)
 *   @param {string}   opts.yLabel     Rotated left-axis label
 *   @param {string}   opts.xAxisLabel Bottom x-axis label
 *   @param {number[]} opts.xLabels    X-axis tick values
 *   @param {number}   opts.xMax       Maximum x domain value (default 255)
 *   @param {string}   [opts.title]    Optional chart title
 *   @param {number}   [opts.ySteps=4] Number of horizontal grid lines
 * @returns {{ chartW, chartH, x0, y0 }} Inner drawing area
 */
export function drawChartAxes(ctx, W, H, pads, opts) {
    const { left, right, top, bottom } = pads;
    const {
        maxVal,
        yLabel,
        xAxisLabel,
        xLabels,
        xMax = 255,
        title,
        ySteps = 4,
    } = opts;

    const chartW = W - left - right;
    const chartH = H - top - bottom;
    const x0 = left;
    const y0 = top;

    // ── Clear + white background ──────────────────────
    ctx.clearRect(0, 0, W, H);
    ctx.fillStyle = '#fff';
    ctx.fillRect(0, 0, W, H);

    // ── Horizontal grid lines ─────────────────────────
    ctx.strokeStyle = '#ddd';
    ctx.lineWidth = 0.5;
    for (let s = 0; s <= ySteps; s++) {
        const y = y0 + chartH - (s / ySteps) * chartH;
        ctx.beginPath(); ctx.moveTo(x0, y); ctx.lineTo(x0 + chartW, y); ctx.stroke();
    }

    // ── Chart border ──────────────────────────────────
    ctx.strokeStyle = '#333';
    ctx.lineWidth = 1;
    ctx.strokeRect(x0, y0, chartW, chartH);

    // ── Title ─────────────────────────────────────────
    if (title) {
        ctx.fillStyle = '#111';
        ctx.font = 'bold 12px Inter, sans-serif';
        ctx.textAlign = 'center';
        ctx.textBaseline = 'top';
        ctx.fillText(title, x0 + chartW / 2, 6);
    }

    // ── Y-axis numbers + ticks ────────────────────────
    ctx.fillStyle = '#333';
    ctx.font = '10px Inter, sans-serif';
    ctx.textAlign = 'right';
    ctx.textBaseline = 'middle';
    for (let s = 0; s <= ySteps; s++) {
        const val = Math.round((maxVal / ySteps) * s);
        const y = y0 + chartH - (s / ySteps) * chartH;
        ctx.fillText(val.toLocaleString(), x0 - 5, y);
        ctx.strokeStyle = '#555'; ctx.lineWidth = 0.5;
        ctx.beginPath(); ctx.moveTo(x0 - 3, y); ctx.lineTo(x0, y); ctx.stroke();
    }

    // ── Y-axis rotated label ──────────────────────────
    ctx.save();
    ctx.fillStyle = '#333';
    ctx.font = '10px Inter, sans-serif';
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';
    ctx.translate(12, y0 + chartH / 2);
    ctx.rotate(-Math.PI / 2);
    ctx.fillText(yLabel, 0, 0);
    ctx.restore();

    // ── X-axis numbers + ticks ────────────────────────
    ctx.fillStyle = '#333';
    ctx.font = '10px Inter, sans-serif';
    ctx.textAlign = 'center';
    ctx.textBaseline = 'top';
    xLabels.forEach(v => {
        const x = x0 + (v / xMax) * chartW;
        ctx.fillText(v, x, y0 + chartH + 4);
        ctx.strokeStyle = '#555'; ctx.lineWidth = 0.5;
        ctx.beginPath(); ctx.moveTo(x, y0 + chartH); ctx.lineTo(x, y0 + chartH + 3); ctx.stroke();
    });

    // ── X-axis label ──────────────────────────────────
    ctx.fillStyle = '#333';
    ctx.font = '10px Inter, sans-serif';
    ctx.textAlign = 'center';
    ctx.textBaseline = 'bottom';
    ctx.fillText(xAxisLabel, x0 + chartW / 2, H - 2);

    return { chartW, chartH, x0, y0 };
}
