function save_table(path, table)

	local file = io.open(path, "w")
	local tabc = 1
	local function nline()
		file:write("\n")
		for i = 1, tabc do
			file:write("\t")
		end
	end

	file:write("local e = {")
	nline()
	local function write_table(t)
		local lv = nil
		for k, v in pairs(t) do
			if type(v) ~= "function" and type(v) ~= "userdata" then
				if lv ~= nil then
					file:write(",")
					nline()
				end
				if type(k) == "string" then
					file:write(k .. " = ")
				end
				if type(v) == "table" then
					file:write("{")
					tabc = tabc + 1
					nline()
					write_table(v)
					tabc = tabc - 1
					nline()
					file:write("}")
				elseif type(v) == "string" then
					file:write("\"" .. v .."\"")
				else
					file:write(v)
				end
				lv = v
			end
		end

	end

	write_table(table)
	tabc = tabc - 1
	nline()

	file:write("}")
	nline()
	file:write("return e")
	file:close()
end

function load_table(path, table)
	local t = dofile(path)

	for k, v in pairs(t) do
		table[k] = v
	end
end