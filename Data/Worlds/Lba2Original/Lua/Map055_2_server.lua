function InitMap(environment)
	Spawn_1 = Spawn(1)
	Spawn_1:SetPosX(9)
	Spawn_1:SetPosY(0)
	Spawn_1:SetPosZ(23)
	Spawn_1:SetName("from_otringal1")
	Spawn_1:SetForceRotation(true)
	Spawn_1:SetRotation(180)
	environment:AddSpawn(Spawn_1)

	Spawn_2 = Spawn(2)
	Spawn_2:SetPosX(4)
	Spawn_2:SetPosY(0)
	Spawn_2:SetPosZ(23)
	Spawn_2:SetName("from_otringal2")
	Spawn_2:SetForceRotation(true)
	Spawn_2:SetRotation(180)
	environment:AddSpawn(Spawn_2)

	Spawn_3 = Spawn(3)
	Spawn_3:SetPosX(9)
	Spawn_3:SetPosY(0)
	Spawn_3:SetPosZ(8)
	Spawn_3:SetName("from_terminal1")
	environment:AddSpawn(Spawn_3)

	Spawn_4 = Spawn(4)
	Spawn_4:SetPosX(4)
	Spawn_4:SetPosY(0)
	Spawn_4:SetPosZ(8)
	Spawn_4:SetName("from_terminal2")
	environment:AddSpawn(Spawn_4)

	Actor_1 = ActorObjectInfo(1)
	Actor_1:SetRenderType(1)
	Actor_1.DisplayDesc.ModelId = 0
	Actor_1.DisplayDesc.ModelName = "Worlds/Lba2Original/Grids/Map55.osgb"
	Actor_1.DisplayDesc.Outfit = ""
	Actor_1.DisplayDesc.Weapon = ""
	Actor_1.DisplayDesc.Mode = ""
	Actor_1.DisplayDesc.UseLight = true
	Actor_1.DisplayDesc.CastShadow = false
	Actor_1.DisplayDesc.ColorR = 0
	Actor_1.DisplayDesc.ColorG = 7.30015e-031
	Actor_1.DisplayDesc.ColorB = 7.375
	Actor_1.DisplayDesc.ColorA = 14.25
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
	Actor_1.PhysicDesc.Density = 1.31224e-038
	Actor_1.PhysicDesc.Collidable = true
	Actor_1.PhysicDesc.SizeX = 0
	Actor_1.PhysicDesc.SizeY = 0
	Actor_1.PhysicDesc.SizeZ = 0
	Actor_1.PhysicDesc.Filename = "Worlds/Lba2Original/Grids/Map55.phy"
	Actor_1:SetPhysicalActorType(1)
	Actor_1:SetPhysicalShape(5)
	Actor_1.ExtraInfo.Name = ""
	Actor_1.ExtraInfo.NameColorR = 9.82776e-031
	Actor_1.ExtraInfo.NameColorG = 0
	Actor_1.ExtraInfo.NameColorB = 0
	Actor_1H = ActorHandler(Actor_1)
	environment:AddActorObject(Actor_1H)

	Trigger_1_info = TriggerInfo(1, "to_otringal1", true, true, false)
	Trigger_1 = ZoneTrigger(Trigger_1_info, 4, 8, 1, true)
	Trigger_1:SetPosition(9, 0, 24.5)
	Trigger_1_act1 = TeleportAction()
	Trigger_1_act1:SetMapName("OTRINGAL")
	Trigger_1_act1:SetSpawning(3)
	Trigger_1:SetAction1(Trigger_1_act1)
	environment:AddTrigger(Trigger_1)

	Trigger_2_info = TriggerInfo(2, "to_otringal2", true, true, false)
	Trigger_2 = ZoneTrigger(Trigger_2_info, 4, 8, 1, true)
	Trigger_2:SetPosition(4, 0, 24.5)
	Trigger_2_act1 = TeleportAction()
	Trigger_2_act1:SetMapName("OTRINGAL")
	Trigger_2_act1:SetSpawning(4)
	Trigger_2:SetAction1(Trigger_2_act1)
	environment:AddTrigger(Trigger_2)

	Trigger_3_info = TriggerInfo(3, "to_terminal1", true, true, false)
	Trigger_3 = ZoneTrigger(Trigger_3_info, 4, 8, 1, true)
	Trigger_3:SetPosition(9, 0, 5.5)
	Trigger_3_act1 = TeleportAction()
	Trigger_3_act1:SetMapName("Map047_2")
	Trigger_3_act1:SetSpawning(3)
	Trigger_3:SetAction1(Trigger_3_act1)
	environment:AddTrigger(Trigger_3)

	Trigger_4_info = TriggerInfo(4, "to_terminal2", true, true, false)
	Trigger_4 = ZoneTrigger(Trigger_4_info, 4, 8, 1, true)
	Trigger_4:SetPosition(4, 0, 5.5)
	Trigger_4_act1 = TeleportAction()
	Trigger_4_act1:SetMapName("Map047_2")
	Trigger_4_act1:SetSpawning(2)
	Trigger_4:SetAction1(Trigger_4_act1)
	environment:AddTrigger(Trigger_4)

end

