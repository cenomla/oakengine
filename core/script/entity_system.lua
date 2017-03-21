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
	sorted_entities = {},
	needs_sort = false,
	events = {},

	create_entity = function(self, layer, thing)
		local e = self.manager.create_entity(layer, thing)
		self.entities[e:index() + 1] = e
		e:activate()
		self:send_message("on_create", e)
		self.needs_sort = true
		return e
	end,

	destroy_entity = function(self, e)
		if self.is_valid(e) then
			self:send_message("on_destroy", e)

			self.entities[e:index() + 1] = nil
			self.manager.destroy_entity(e)
			self.needs_sort = false
		end
	end,

	destroy_all = function(self)
		for k, v in pairs(self.entities) do
			self:destroy_entity(v)
		end
	end,

	get_entity = function(self, e)
		if type(e) == "number" then
			return self.entities[e + 1]
		end
		return e
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
		e = self:get_entity(e)
		if self.is_valid(e) and e[event_name] then 
			e[event_name](e, ...) 
		end
	end,

	sort_entities = function(self)
		--sort entities is needed
		if self.needs_sort then
			--clear sorted entity list
			self.sorted_entities = {}
			--populate it
			for k, v in pairs(self.entities) do
				if v ~= nil then
					table.insert(self.sorted_entities, v)
				end
			end

			--sort entities
			table.sort(self.sorted_entities, function(e, o) 
				return e:layer() > o:layer() 
			end)
			self.needs_sort = false
		end
	end,

	process_events = function(self)
		self:sort_entities()
		--process events
		for name, v in pairs(self.events) do
			for k, evt in pairs(v) do
				self:process_event(self.sorted_entities, name, table.unpack(evt))
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