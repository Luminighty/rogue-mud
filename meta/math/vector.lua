require("meta.math.utils")

local component_names = { "x", "y", "z", "w" }

local function vector_typedef(vector)
	if not IS_HEADER then
		return
	end
	io.write("typedef union {\n")
	local fields = {}
	for i = 1, vector.dimension do
		table.insert(fields, string.format("%s %s;", vector.component, component_names[i]))
	end
	io.write(string.format("\tstruct { %s };\n", table.concat(fields, " ")))
	io.write(string.format("\t%s v[%d];\n", vector.component, vector.dimension))
	io.write(string.format("} %s;\n\n", vector.name))
end

local function vector_op(vector, method_name, op)
	FunctionBegin(vector.name, vector.prefix .. "_" .. method_name, vector.name .. " a", vector.name .. " b")
	if IS_HEADER then
		return
	end

	local fields = {}
	for i = 1, vector.dimension do
		local field = component_names[i]
		table.insert(fields, string.format("\t\t.%s = a.%s %s b.%s,\n", field, field, op, field))
	end
	io.write(string.format("\treturn ((%s){\n%s\t});\n}\n", vector.name, table.concat(fields)))
end

local function vector_scale(vector)
	FunctionBegin(vector.name, vector.prefix .. "_scale", vector.name .. " a", vector.component .. " scale")
	if IS_HEADER then
		return
	end

	local fields = {}
	for i = 1, vector.dimension do
		local field = component_names[i]
		table.insert(fields, string.format("\t\t.%s = a.%s * scale,\n", field, field))
	end
	io.write(string.format("\treturn ((%s){\n%s\t});\n}\n", vector.name, table.concat(fields)))
end

local function vector_dot(vector)
	FunctionBegin(vector.component, vector.prefix .. "_dot", vector.name .. " a", vector.name .. " b")
	if IS_HEADER then
		return
	end

	local fields = {}
	for i = 1, vector.dimension do
		local field = component_names[i]
		table.insert(fields, string.format("(a.%s * b.%s)", field, field))
	end
	io.write(string.format("\treturn %s;\n}\n", table.concat(fields, " + ")))
end

local function vector_lengthsqr(vector)
	FunctionBegin(vector.component, vector.prefix .. "_lengthsqr", vector.name .. " vector")
	if IS_HEADER then
		return
	end

	local fields = {}
	for i = 1, vector.dimension do
		local field = component_names[i]
		table.insert(fields, string.format("(vector.%s * vector.%s)", field, field))
	end
	io.write(string.format("\treturn %s;\n}\n", table.concat(fields, " + ")))
end

local function vector_normalize(vector)
	FunctionBegin(vector.name, vector.prefix .. "_normalize", vector.name .. " vector")
	if IS_HEADER then
		return
	end

	local fields = {}
	for i = 1, vector.dimension do
		local field = component_names[i]
		table.insert(fields, string.format("\t\t.%s = (%s)(vector.%s * inv_len),\n", field, vector.component, field))
	end
	io.write(string.format("\tfloat inv_len = fast_inv_sqrt(%s_lengthsqr(vector));\n", vector.prefix))
	io.write(string.format("\treturn ((%s){\n%s\t});\n}\n", vector.name, table.concat(fields)))
end

local function vector_print(vector)
	FunctionBegin("void", vector.prefix .. "_print", vector.name .. " vector")
	if IS_HEADER then
		return
	end

	local fmt_map = { int = "%d", float = "%f" }
	local formats = {}
	local values = {}
	for i = 1, vector.dimension do
		table.insert(formats, fmt_map[vector.component])
		table.insert(values, "vector." .. component_names[i])
	end
	io.write(
		string.format(
			'\tprintf("%s(%s)", %s);\n}\n',
			vector.name,
			table.concat(formats, ", "),
			table.concat(values, ", ")
		)
	)
end

local function vector_fn(vector, fn_name, fn_to_call)
	FunctionBegin(vector.name, vector.prefix .. "_" .. fn_name, vector.name .. " a", vector.name .. " b")
	if IS_HEADER then
		return
	end

	local fields = {}
	for i = 1, vector.dimension do
		local field = component_names[i]
		table.insert(fields, string.format("\t\t.%s = %s(a.%s, b.%s),\n", field, fn_to_call, field, field))
	end
	io.write(string.format("\treturn ((%s){\n%s\t});\n}\n", vector.name, table.concat(fields)))
end

local function vector_const(vector, name, value)
	if not IS_HEADER then
		return
	end
	local head_format = "#define %s_%s \t"
	io.write(string.format(head_format, string.upper(vector.prefix), string.upper(name)))

	io.write("((" .. vector.name .. "){ ")
	for i = 1, vector.dimension do
		local field = component_names[i]
		io.write(string.format(".%s = %d, ", field, value[i]))
	end
	io.write("})\n")
end

function Vector(vector)
	vector.prefix = vector.prefix or string.lower(vector.name)

	io.write("// ////// ", vector.name, " //////\n")
	vector_typedef(vector)
	vector_op(vector, "add", "+")
	vector_op(vector, "sub", "-")
	vector_scale(vector)
	vector_dot(vector)
	vector_lengthsqr(vector)
	vector_normalize(vector)
	vector_fn(vector, "min", "min")
	vector_fn(vector, "max", "max")
	vector_print(vector)

	vector_const(vector, "zero", { 0, 0, 0, 0 })
	vector_const(vector, "one", { 1, 1, 1, 1 })
	vector_const(vector, "left", { -1, 0, 0, 0 })
	vector_const(vector, "right", { 1, 0, 0, 0 })
	vector_const(vector, "up", { 0, -1, 0, 0 })
	vector_const(vector, "down", { 0, 1, 0, 0 })

	io.write("\n\n")
end
