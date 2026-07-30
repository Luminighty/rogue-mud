require("meta.math.utils")

local function assert_offset(rect, field, other_field)
	local fmt = ('static_assert(offsetof(Rect2i, %s) == offsetof(Rect2i, %s), "Offset mismatch for %s and %s");\n'):format(
		field,
		other_field,
		field,
		other_field
	)
	io.write(fmt)
end

local function rect_typedef(rect)
	if not IS_HEADER then
		assert_offset(rect, "x", "position.x")
		assert_offset(rect, "y", "position.y")
		assert_offset(rect, "w", "size.x")
		assert_offset(rect, "h", "size.y")
		return
	end
	local vec_map = { int = "Vec2i", float = "Vec2" }
	local vec = vec_map[rect.component]
	io.write("typedef union {\n")
	io.write(("\tstruct { %s pos; %s size; };\n"):format(vec, vec))
	io.write("\tstruct {\n")
	io.write(("\t\t%s x; %s y;\n"):format(rect.component, rect.component))
	io.write(("\t\t%s w; %s h;\n"):format(rect.component, rect.component))
	io.write("\t};\n")
	io.write(("} %s;\n\n"):format(rect.name))
end

local function rect_fmt(rect)
	if not IS_HEADER then
		return
	end

	local fmt_map = { int = "%d", float = "%f" }
	local fmt = fmt_map[rect.component]
	local formats = table.concat({
		"x: " .. fmt,
		"y: " .. fmt,
		"w: " .. fmt,
		"h: " .. fmt,
	}, ", ")
	local values = table.concat({
		"(rect).x",
		"(rect).y",
		"(rect).w",
		"(rect).h",
	}, ", ")
	local name = string.upper(rect.name)
	io.write(string.format('#define %s_FMT "%s(%s)"\n', name, rect.name, formats))
	io.write(string.format("#define %s_ARG(rect) %s\n", name, values))
end

local function rect_contains(rect)
	FunctionBegin(
		"bool",
		rect.prefix .. "_contains",
		rect.name .. " rect",
		rect.component .. " x",
		rect.component .. " y"
	)
	if IS_HEADER then
		return
	end
	local ops = table.concat({
		"x >= rect.x",
		"y >= rect.y",
		"x <= rect.x + rect.w",
		"y <= rect.y + rect.h",
	}, " &&\n\t\t")
	io.write("\treturn " .. ops .. ";\n}\n")
end

local function rect_center(rect)
	local vec2_ty = ({ int = "Vec2i", float = "Vec2" })[rect.component]
	FunctionBegin(vec2_ty, rect.prefix .. "_center", rect.name .. " rect")
	if IS_HEADER then
		return
	end
	local half = ({ int = "2", float = "2.0f" })[rect.component]
	local vec = {
		".x = rect.x + rect.w / " .. half,
		".y = rect.y + rect.h / " .. half,
	}
	io.write(string.format("\treturn ((%s){ %s, %s });\n}\n", vec2_ty, table.unpack(vec)))
end

local function rect_overlaps(rect)
	FunctionBegin("bool", rect.prefix .. "_overlaps", rect.name .. " a", rect.name .. " b")
	if IS_HEADER then
		return
	end
	local ops = table.concat({
		"(a.x < b.x + b.w)",
		"(a.x + a.w > b.x)",
		"(a.y < b.y + b.h)",
		"(a.y + a.h > b.y)",
	}, " &&\n\t\t")
	io.write("\treturn " .. ops .. ";\n}\n")
end

function Rect2(rect)
	rect.prefix = rect.prefix or string.lower(rect.name)

	io.write("// ////// ", rect.name, " //////\n")
	rect_typedef(rect)
	rect_fmt(rect)
	rect_overlaps(rect)
	rect_contains(rect)
	rect_center(rect)

	io.write("\n\n")
end
