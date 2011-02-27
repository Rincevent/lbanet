function InitMap(environment)
	Spawn_1 = Spawn(1)
	Spawn_1:SetPosX(15)
	Spawn_1:SetPosY(9)
	Spawn_1:SetPosZ(12)
	Spawn_1:SetName("from_citadelfirst")
	Spawn_1:SetForceRotation(true)
	Spawn_1:SetRotation(270)
	environment:AddSpawn(Spawn_1)

	Spawn_2 = Spawn(2)
	Spawn_2:SetPosX(15)
	Spawn_2:SetPosY(4)
	Spawn_2:SetPosZ(35)
	Spawn_2:SetName("from_citadelsecond")
	Spawn_2:SetForceRotation(true)
	Spawn_2:SetRotation(270)
	environment:AddSpawn(Spawn_2)

	Actor_1 = ActorObjectInfo(1)
	Actor_1:SetRenderType(1)
	Actor_1.DisplayDesc.ModelId = -1163005939
	Actor_1.DisplayDesc.ModelName = "Worlds/Lba2Original/Grids/Map43.osgb"
	Actor_1.DisplayDesc.Outfit = ""
	Actor_1.DisplayDesc.Weapon = ""
	Actor_1.DisplayDesc.Mode = ""
	Actor_1.DisplayDesc.UseLight = true
	Actor_1.DisplayDesc.CastShadow = false
	Actor_1.DisplayDesc.ColorR = -0.00132704
	Actor_1.DisplayDesc.ColorG = -0.00132704
	Actor_1.DisplayDesc.ColorB = -0.00132704
	Actor_1.DisplayDesc.ColorA = -0.00132704
	Actor_1:SetModelState(1)
	Actor_1.PhysicDesc.Pos.X = 0
	Actor_1.PhysicDesc.Pos.Y = 0
	Actor_1.PhysicDesc.Pos.Z = 0
	Actor_1.PhysicDesc.Pos.Rotation = 0
	Actor_1.PhysicDesc.Density = -0.00132704
	Actor_1.PhysicDesc.Collidable = true
	Actor_1.PhysicDesc.SizeX = -0.00132704
	Actor_1.PhysicDesc.SizeY = -0.00132704
	Actor_1.PhysicDesc.SizeZ = -0.00132704
	Actor_1.PhysicDesc.Filename = "Worlds/Lba2Original/Grids/Map43.phy"
	Actor_1:SetPhysicalActorType(1)
	Actor_1:SetPhysicalShape(5)
	Actor_1.ExtraInfo.Name = ""
	Actor_1.ExtraInfo.NameColorR = -0.00132704
	Actor_1.ExtraInfo.NameColorG = -0.00132704
	Actor_1.ExtraInfo.NameColorB = -0.00132704
	Actor_1H = ActorHandler(Actor_1)
	environment:AddActorObject(Actor_1H)

	Trigger_1_info = TriggerInfo(1, "to_citadelfirst", true, true, false)
	Trigger_1 = ZoneTrigger(Trigger_1_info, 1, 8, 2, true)
	Trigger_1:SetPosition(17.5, 9, 12)
	Trigger_1_act1 = TeleportAction()
	Trigger_1_act1:SetMapName("CITABAU")
	Trigger_1_act1:SetSpawning(13)
	Trigger_1:SetAction1(Trigger_1_act1)
	environment:AddTrigger(Trigger_1)

	Trigger_2_info = TriggerInfo(2, "to_citadelsecond", true, true, false)
	Trigger_2 = ZoneTrigger(Trigger_2_info, 1, 8, 2, true)
	Trigger_2:SetPosition(16.5, 4, 35)
	Trigger_2_act1 = TeleportAction()
	Trigger_2_act1:SetMapName("CITABAU")
	Trigger_2_act1:SetSpawning(14)
	Trigger_2:SetAction1(Trigger_2_act1)
	environment:AddTrigger(Trigger_2)

end

