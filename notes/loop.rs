width := 10;
height := 10;
depth := 10;

voxels [10 * [10 * [10 * bool = false]]];

loop(x := 1; x <= width ;; x += 1)
loop(y := 1; y <= height;; y += 1)
loop(z := 1; z <= depth ;; z += 1)
    voxels.(x).(y).(z) = true;

for x := 1; x <= width ; x += 1 do
for y := 1; y <= height; y += 1 do
for z := 1; z <= depth ; z += 1 do
    voxels.(x).(y).(z) = true;
    