function InitMap(environment)
	MapObject = ActorObjectInfo(1)
	MapObject:SetRenderType(1)
	MapObject.DisplayDesc.ModelName = "Worlds/Lba1Original/Grids/Map35.osgb"
	MapObject.DisplayDesc.UseLight = true
	MapObject.DisplayDesc.CastShadow = false
	MapObject:SetModelState(1)
	MapObject.PhysicDesc.Pos.X = 0
	MapObject.PhysicDesc.Pos.Y = 0
	MapObject.PhysicDesc.Pos.Z = 0
	MapObject.PhysicDesc.Pos.Rotation = 0
	MapObject:SetPhysicalActorType(1)
	MapObject:SetPhysicalShape(5)
	MapObject.PhysicDesc.Collidable = true
	MapObject.PhysicDesc.Filename = "Worlds/Lba1Original/Grids/Map35.phy"
	MapObjectH = ActorHandler(MapObject)
	environment:AddActorObject(MapObjectH)

	Trigger_1_info = TriggerInfo(1, "exit0", true, true, false)
	Trigger_1 = ZoneTrigger(Trigger_1_info, 3, 4.5, 1, true)
	Trigger_1:SetPosition(53.5, 0, 63.5)
	Trigger_1:SetAction1(107)
	Trigger_1:SetAction2(-1)
	Trigger_1:SetAction3(-1)
	environment:AddTrigger(Trigger_1)

	Trigger_2_info = TriggerInfo(2, "exit1", true, true, false)
	Trigger_2 = ZoneTrigger(Trigger_2_info, 1, 1.5, 1, true)
	Trigger_2:SetPosition(3.5, 0, 33.5)
	Trigger_2:SetAction1(108)
	Trigger_2:SetAction2(-1)
	Trigger_2:SetAction3(-1)
	environment:AddTrigger(Trigger_2)

end
