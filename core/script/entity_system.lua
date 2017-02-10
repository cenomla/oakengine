function iter(a, i)
	i = i + 1
	if i <= #a then
		return i, a[i]
	end
end

function ipairs(a)
	return iter, a, 0
end

local entity_system = {

	entities = {},
	events = {},

	create_prefab = function(self, name, prefab)
		self.manager:create_prefab(name, prefab)
	end,

	create_entity = function(self, ...)
		local e = self.manager.create_entity(...)
		if self.entity_metatable == nil then
			local mt = getmetatable(e)
			if mt ~= nil then
				self.entity_metatable = getmetatable(mt)
			end
		end
		self.entities[e:index() + 1] = e
		self:send_message("on_create", e)
		e:activate()
		return e
	end,

	destroy_entity = function(self, e)
		if self.is_valid(e) then
			self:send_message("on_destroy", e)

			self.entities[e:index() + 1] = nil
			self.manager.destroy_entity(e)
		end
	end,

	destroy_all = function(self)
		for k, v in pairs(self.entities) do
			self:destroy_entity(v)
		end
	end,

	get_entity = function(self, e)
		local index = nil
		if type(e) == "number" then
			index = e
		elseif type(e) == "userdata" and self.entity_metatable ~= nil then
			local t = { _id = e }
			setmetatable(t, self.entity_metatable)
			index = t:index()
		else 
			return nil
		end
		return self.entities[index + 1]
	end,

	is_valid = function(e)
		return e ~= nil
	end,

	emit_event = function(self, event_name, ...)
		if not self.events[event_name] then
			self.events[event_name] = {}
		end
		table.insert(self.events[event_name], { ... })
	end,

	send_message = function(self, event_name, e, ...)
		if type(e) ~= "table" then e = self:get_entity(e) end
		if self.is_valid(e) and e[event_name] then 
			e[event_name](e, ...) 
		end
	end,

	send_event = function(self, event_name, event)
		self:send_message(event_name, event.entity, event)
	end,

	process_events = function(self)
		--create a list of entities that can be sorted
		local sorted = {}
		for k, v in pairs(self.entities) do
			if v ~= nil then
				table.insert(sorted, v)
			end
		end

		--sort entities
		table.sort(sorted, function(e, o) 
			return e:layer() > o:layer() 
		end)
		--process events
		for name, v in pairs(self.events) do
			for k, evt in pairs(v) do
				self:process_event(sorted, name, table.unpack(evt))
			end
			self.events[name] = {}
		end
	end,

	process_event = function(self, list, event_name, ...)
		--call events
		for k, v in ipairs(list) do
			if self.is_valid(v) and v:is_active() and v[event_name] then
				--if the event function returns true that means it wants to capture the event so return
				if v[event_name](v, ...) then return end
			end
		end
	end

}

return entity_system