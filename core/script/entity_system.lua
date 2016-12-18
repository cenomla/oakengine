function iter(a, i)
	i = i - 1
	local v = a[i]
	if v then
		return i, v
	end
end

function rpairs(a)
	return iter, a, #a + 1
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

	destroyEntity = function(self, e)
		self:send_message("destroy", e)
		self.entities[e:index() + 1] = nil
		self.manager.destroyEntity(e)
	end,

	destroyAll = function(self)
		for k, v in pairs(self.entities) do
			self:destroyEntity(v)
		end
	end,

	get_entity = function(self, index)
		return self.entities[index + 1]
	end,

	emit_event = function(self, event_name, ...)
		if not self.events[event_name] then
			self.events[event_name] = {}
		end
		table.insert(self.events[event_name], { ... })
	end,

	send_message = function(self, event_name, e, ...)
		if e[event_name] then e[event_name](e, ...) end
	end,

	process_events = function(self)		
		for name, v in pairs(self.events) do
			for k, evt in pairs(v) do
				self:process_event(name, table.unpack(evt))
			end
			self.events[name] = {}
		end
	end,

	process_event = function(self, event_name, ...)
		--sort entities
		local sorted = self.entities
		table.sort(sorted, function(e, o) return e:layer() < o:layer() end)
		--call events
		for k, v in rpairs(sorted) do
			if v:isActive() and v[event_name] then
				--if the event function returns true that means it wants to capture the event so return
				if v[event_name](v, ...) then return end
			end
		end
	end

}

return entity_system