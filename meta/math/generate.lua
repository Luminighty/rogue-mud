require("meta.math.utils")
require("meta.math.vector")
require("meta.math.basic")

local header_guard = "LINALG_H"
local filename = "linalg"
local src_includes = { "<stdio.h>" }

local function generate()
	Basic()

	Vector({ name = "Vec2i", component = "int", dimension = 2 })
	Vector({ name = "Vec3i", component = "int", dimension = 3 })
	Vector({ name = "Vec2", component = "float", dimension = 2 })
	Vector({ name = "Vec3", component = "float", dimension = 3 })
end

return function()
	-- Header File Gen
	local header_file = filename .. ".h"
	io.output("./include/" .. header_file)
	MathHeaderBegin(header_guard)
	generate()
	MathHeaderEnd(header_guard)
	io.close()

	-- Source File Gen
	io.output("./src/" .. filename .. ".c")
	MathSourceBegin(header_file, src_includes)
	generate()
	MathSourceEnd()
end
