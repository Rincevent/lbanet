function InitMap(environment)
	Spawn_1 = Spawn(1)
	Spawn_1:SetPosX(34)
	Spawn_1:SetPosY(4)
	Spawn_1:SetPosZ(34)
	Spawn_1:SetName("FirstSpawning")
	environment:AddSpawn(Spawn_1)

	Spawn_2 = Spawn(2)
	Spawn_2:SetPosX(35)
	Spawn_2:SetPosY(13)
	Spawn_2:SetPosZ(10)
	Spawn_2:SetName("spawning1")
	environment:AddSpawn(Spawn_2)

	Spawn_3 = Spawn(3)
	Spawn_3:SetPosX(11)
	Spawn_3:SetPosY(0)
	Spawn_3:SetPosZ(46)
	Spawn_3:SetName("spawning2")
	environment:AddSpawn(Spawn_3)

	Spawn_4 = Spawn(4)
	Spawn_4:SetPosX(2.5)
	Spawn_4:SetPosY(-1)
	Spawn_4:SetPosZ(0.5)
	Spawn_4:SetName("spawning3")
	environment:AddSpawn(Spawn_4)

	Spawn_5 = Spawn(5)
	Spawn_5:SetPosX(0.5)
	Spawn_5:SetPosY(-1)
	Spawn_5:SetPosZ(0.5)
	Spawn_5:SetName("spawning4")
	environment:AddSpawn(Spawn_5)

	Spawn_6 = Spawn(6)
	Spawn_6:SetPosX(0.5)
	Spawn_6:SetPosY(-1)
	Spawn_6:SetPosZ(0.5)
	Spawn_6:SetName("spawning5")
	environment:AddSpawn(Spawn_6)

	Spawn_7 = Spawn(7)
	Spawn_7:SetPosX(0.5)
	Spawn_7:SetPosY(-1)
	Spawn_7:SetPosZ(0.5)
	Spawn_7:SetName("spawning6")
	environment:AddSpawn(Spawn_7)

	Spawn_8 = Spawn(8)
	Spawn_8:SetPosX(0.5)
	Spawn_8:SetPosY(-1)
	Spawn_8:SetPosZ(0.5)
	Spawn_8:SetName("spawning7")
	environment:AddSpawn(Spawn_8)

	Spawn_9 = Spawn(9)
	Spawn_9:SetPosX(0.5)
	Spawn_9:SetPosY(-1)
	Spawn_9:SetPosZ(1)
	Spawn_9:SetName("spawning8")
	environment:AddSpawn(Spawn_9)

	Actor_1 = ActorObjectInfo(1)
	Actor_1:SetRenderType(1)
	Actor_1.DisplayDesc.ModelId = -842150451
	Actor_1.DisplayDesc.ModelName = "Worlds/Lba1Original/Grids/Map0.osgb"
	Actor_1.DisplayDesc.Outfit = ""
	Actor_1.DisplayDesc.Weapon = ""
	Actor_1.DisplayDesc.Mode = ""
	Actor_1.DisplayDesc.UseLight = true
	Actor_1.DisplayDesc.CastShadow = false
	Actor_1.DisplayDesc.ColorR = -4.31602e+008
	Actor_1.DisplayDesc.ColorG = -4.31602e+008
	Actor_1.DisplayDesc.ColorB = -4.31602e+008
	Actor_1.DisplayDesc.ColorA = -4.31602e+008
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
	Actor_1.PhysicDesc.Density = -4.31602e+008
	Actor_1.PhysicDesc.Collidable = true
	Actor_1.PhysicDesc.SizeX = -4.31602e+008
	Actor_1.PhysicDesc.SizeY = -4.31602e+008
	Actor_1.PhysicDesc.SizeZ = -4.31602e+008
	Actor_1.PhysicDesc.Filename = "Worlds/Lba1Original/Grids/Map0.phy"
	Actor_1:SetPhysicalActorType(1)
	Actor_1:SetPhysicalShape(5)
	Actor_1.ExtraInfo.Name = ""
	Actor_1.ExtraInfo.NameColorR = -4.31602e+008
	Actor_1.ExtraInfo.NameColorG = -4.31602e+008
	Actor_1.ExtraInfo.NameColorB = -4.31602e+008
	Actor_1H = ActorHandler(Actor_1)
	environment:AddActorObject(Actor_1H)

	Actor_2 = ActorObjectInfo(2)
	Actor_2:SetRenderType(3)
	Actor_2.DisplayDesc.ModelId = 0
	Actor_2.DisplayDesc.ModelName = "Sphero"
	Actor_2.DisplayDesc.Outfit = "Yellow"
	Actor_2.DisplayDesc.Weapon = "No"
	Actor_2.DisplayDesc.Mode = "Normal"
	Actor_2.DisplayDesc.UseLight = true
	Actor_2.DisplayDesc.CastShadow = true
	Actor_2.DisplayDesc.ColorR = 1
	Actor_2.DisplayDesc.ColorG = 1
	Actor_2.DisplayDesc.ColorB = 1
	Actor_2.DisplayDesc.ColorA = 1
	Actor_2.DisplayDesc.TransX = 0
	Actor_2.DisplayDesc.TransY = 0
	Actor_2.DisplayDesc.TransZ = 0
	Actor_2.DisplayDesc.ScaleX = 1
	Actor_2.DisplayDesc.ScaleY = 1
	Actor_2.DisplayDesc.ScaleZ = 1
	Actor_2.DisplayDesc.RotX = 0
	Actor_2.DisplayDesc.RotY = 0
	Actor_2.DisplayDesc.RotZ = 0
	Actor_2:SetModelState(2)
	Actor_2.PhysicDesc.Pos.X = 34
	Actor_2.PhysicDesc.Pos.Y = 4
	Actor_2.PhysicDesc.Pos.Z = 34.5
	Actor_2.PhysicDesc.Pos.Rotation = 0
	Actor_2.PhysicDesc.Density = 1
	Actor_2.PhysicDesc.Collidable = true
	Actor_2.PhysicDesc.SizeX = 1
	Actor_2.PhysicDesc.SizeY = 3
	Actor_2.PhysicDesc.SizeZ = 1
	Actor_2.PhysicDesc.Filename = ""
	Actor_2:SetPhysicalActorType(1)
	Actor_2:SetPhysicalShape(2)
	Actor_2.ExtraInfo.Name = ""
	Actor_2.ExtraInfo.NameColorR = 1
	Actor_2.ExtraInfo.NameColorG = 1
	Actor_2.ExtraInfo.NameColorB = 1
	Actor_2H = ActorHandler(Actor_2)
	environment:AddActorObject(Actor_2H)

	Trigger_1_info = TriggerInfo(1, "exit0", true, true, false)
	Trigger_1 = ZoneTrigger(Trigger_1_info, 1, 4, 5, true)
	Trigger_1:SetPosition(37.5, 13, 12.5)
	Trigger_1_act1 = TeleportAction()
	Trigger_1_act1:SetMapName("Map001_1")
	Trigger_1_act1:SetSpawning(1)
	Trigger_1:SetAction1(Trigger_1_act1)
	environment:AddTrigger(Trigger_1)

	Trigger_2_info = TriggerInfo(2, "exit17", true, true, false)
	Trigger_2 = ZoneTrigger(Trigger_2_info, 2, 3, 1, true)
	Trigger_2:SetPosition(11, 0, 43.5)
	Trigger_2_act1 = TeleportAction()
	Trigger_2_act1:SetMapName("Map001_1")
	Trigger_2_act1:SetSpawning(2)
	Trigger_2:SetAction1(Trigger_2_act1)
	environment:AddTrigger(Trigger_2)

	Trigger_3_info = TriggerInfo(3, "dfdgdfg", true, true, false)
	Trigger_3 = ZoneTrigger(Trigger_3_info, 1, 1, 1, true)
	Trigger_3:SetPosition(23, 4, 35)
	Trigger_3_act1 = OpenContainerAction()
	Trigger_3_act1:SetTimeToReset(600)
		ContItem0 = ContainerItemGroupElement(17,1,1,1,-1)
		Trigger_3_act1:AddItem(ContItem0)
		ContItem1 = ContainerItemGroupElement(21,1,1,1,-1)
		Trigger_3_act1:AddItem(ContItem1)
	Trigger_3:SetAction1(Trigger_3_act1)
	environment:AddTrigger(Trigger_3)

end

