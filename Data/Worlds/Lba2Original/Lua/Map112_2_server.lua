function InitMap(environment)
	Spawn_1 = Spawn(1)
	Spawn_1:SetPosX(51)
	Spawn_1:SetPosY(1)
	Spawn_1:SetPosZ(9)
	Spawn_1:SetName("from_inmain")
	environment:AddSpawn(Spawn_1)

	Spawn_2 = Spawn(2)
	Spawn_2:SetPosX(61)
	Spawn_2:SetPosY(1)
	Spawn_2:SetPosZ(9)
	Spawn_2:SetName("from_outmain")
	Spawn_2:SetForceRotation(true)
	Spawn_2:SetRotation(270)
	environment:AddSpawn(Spawn_2)

	Actor_1 = ActorObjectInfo(1)
	Actor_1:SetRenderType(1)
	Actor_1.DisplayDesc.ModelId = 750
	Actor_1.DisplayDesc.ModelName = "Worlds/Lba2Original/Grids/Map112.osgb"
	Actor_1.DisplayDesc.Outfit = ""
	Actor_1.DisplayDesc.Weapon = ""
	Actor_1.DisplayDesc.Mode = ""
	Actor_1.DisplayDesc.UseLight = true
	Actor_1.DisplayDesc.CastShadow = false
	Actor_1.DisplayDesc.ColorR = 0
	Actor_1.DisplayDesc.ColorG = 0
	Actor_1.DisplayDesc.ColorB = 2.10195e-044
	Actor_1.DisplayDesc.ColorA = 5.03608e-024
	Actor_1.DisplayDesc.TransX = 0
	Actor_1.DisplayDesc.TransY = 0
	Actor_1.DisplayDesc.TransZ = 0
	Actor_1.DisplayDesc.ScaleX = 1
	Actor_1.DisplayDesc.ScaleY = 1
	Actor_1.DisplayDesc.ScaleZ = 1
	Actor_1.DisplayDesc.RotX = 0
	Actor_1.DisplayDesc.RotY = 0
	Actor_1.DisplayDesc.RotZ = 0
	Actor_1:SetModelState(1)
	Actor_1.PhysicDesc.Pos.X = 0
	Actor_1.PhysicDesc.Pos.Y = 0
	Actor_1.PhysicDesc.Pos.Z = 0
	Actor_1.PhysicDesc.Pos.Rotation = 0
	Actor_1.PhysicDesc.Density = 0
	Actor_1.PhysicDesc.Collidable = true
	Actor_1.PhysicDesc.SizeX = 0
	Actor_1.PhysicDesc.SizeY = 0
	Actor_1.PhysicDesc.SizeZ = 0
	Actor_1.PhysicDesc.Filename = "Worlds/Lba2Original/Grids/Map112.phy"
	Actor_1:SetPhysicalActorType(1)
	Actor_1:SetPhysicalShape(5)
	Actor_1.ExtraInfo.Name = ""
	Actor_1.ExtraInfo.NameColorR = 0
	Actor_1.ExtraInfo.NameColorG = 7.64534e-031
	Actor_1.ExtraInfo.NameColorB = 0
	Actor_1H = ActorHandler(Actor_1)
	environment:AddActorObject(Actor_1H)

	Trigger_1_info = TriggerInfo(1, "to_inmain", true, true, false)
	Trigger_1 = ZoneTrigger(Trigger_1_info, 1, 8, 2, true)
	Trigger_1:SetPosition(48.5, 1, 9)
	Trigger_1_act1 = TeleportAction()
	Trigger_1_act1:SetMapName("Map014_2")
	Trigger_1_act1:SetSpawning(4)
	Trigger_1:SetAction1(Trigger_1_act1)
	environment:AddTrigger(Trigger_1)

	Trigger_2_info = TriggerInfo(2, "to_outmain", true, true, false)
	Trigger_2 = ZoneTrigger(Trigger_2_info, 1, 8, 4, true)
	Trigger_2:SetPosition(63.5, 1, 9)
	Trigger_2_act1 = TeleportAction()
	Trigger_2_act1:SetMapName("EMERAUDE")
	Trigger_2_act1:SetSpawning(7)
	Trigger_2:SetAction1(Trigger_2_act1)
	environment:AddTrigger(Trigger_2)

end

