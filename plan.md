# Plan

- Add weighted avg
- Use some sort of guess of pixel size to real world size (maybe a calibration process)
- Cluster, dot rating for calibration
- FindSeed return optional
- Maybe reject clusters that are max size
- Use ImGui to draw results overlay
- Add instrumetation to pipeline steps
- Add output graphs for cluster search, fill
- Refactor cluster code
- Remove OpenCV dependency
	- Replacement for Point
	- Replacement for Mat as an image buffer (1 or 3 channel)
		- One that owns data and one as a view
	- Replacement for data manipulation