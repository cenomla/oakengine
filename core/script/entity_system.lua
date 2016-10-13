
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

	createEntity = function(self, layer, name)
		local e = Scene:createEntity(layer, name)
		e.id:activate()
		for i = 1, layer + 1 do
			if not self.entities[i] then
				self.entities[i] = {}
			end
		end
		self.entities[layer + 1][e.id:index() + 1] = e
		return e
	end,

	destroyEntity = function(self, e)
		self.entities[e.id:getLayer() + 1][e.id:index() + 1] = nil
		Scene:destroyEntity(e.id)
	end,

	destroyAll = function(self)
		for kl, l in pairs(self.entities) do
			for kv, v in pairs(l) do
				self:destroyEntity(v)
			end
		end
	end,

	getEntity = function(self, id)
		return self.entities[id:getLayer() + 1][id:index() + 1]
	end,

	emit_event = function(self, event_name, ...)
		if not self.events[event_name] then
			self.events[event_name] = {}
		end
		table.insert(self.events[event_name], ...)
	end,

	send_message = function(self, event_name, id, ...)
		local e = self.entities[id:getLayer() + 1][id:index() + 1]
		if e[event_name] then e[event_name](e, ...) end
	end,

	process_events = function(self)
		--keep all of the entities sorted by their depth so that the entities closest to the screen get their events triggered first
		
		for k, v in pairs(self.events) do
			self:process_event(k, v)
			self.events[k] = {}
		end
	end,

	process_event = function(self, event_name, ...)
		for kl, l in rpairs(self.entities) do
			for kv, v in rpairs(l) do
				if v.id:isActive() and v[event_name] then
					if v[event_name](v, ...) then return end
				end
			end
		end
	end

}

return entity_system