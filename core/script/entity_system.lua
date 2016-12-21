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
	messages = {},

	create_entity = function(self, layer, prefab)
		local e = self.manager.createEntity(layer, prefab)
		e:activate()
		self.entities[e:index() + 1] = e
		self:send_message("create", e)
		return e
	end,

	destroy_entity = function(self, e)
		if self.is_valid(e) then
			self:send_message("destroy", e)

			self.entities[e:index() + 1] = nil
			self.manager.destroyEntity(e)
		end
	end,

	destroy_all = function(self)
		for k, v in pairs(self.entities) do
			self:destroyEntity(v)
		end
	end,

	get_entity = function(self, index)
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
		if self.is_valid(e) and e[event_name] then e[event_name](e, ...) end
	end,

	process_events = function(self)
		--create a list of entities that can be sorted
		local sorted = {}
		for k, v in ipairs(self.entities) do
			table.insert(sorted, v)
		end

		--sort entities
		table.sort(sorted, function(e, o)
			local i1, i2
			if self.is_valid(e) then i1 = e:layer() else i1 = -2e9 end
			if self.is_valid(o) then i2 = o:layer() else i2 = -2e9 end
			return i1 > i2
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
			if self.is_valid(v) and v:isActive() and v[event_name] then
				--if the event function returns true that means it wants to capture the event so return
				if v[event_name](v, ...) then return end
			end
		end
	end

}

return entity_system