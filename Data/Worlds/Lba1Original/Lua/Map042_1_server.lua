function InitMap(environment)
	MapObject = ActorObjectInfo(1)
	MapObject:SetRenderType(1)
	MapObject.DisplayDesc.ModelName = "Worlds/Lba1Original/Grids/Map42.osgb"
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
	MapObject.PhysicDesc.Filename = "Worlds/Lba1Original/Grids/Map42.phy"
	MapObjectH = ActorHandler(MapObject)
	environment:AddActorObject(MapObjectH)

	Trigger_1_info = TriggerInfo(1, "exit14", true, true, false)
	Trigger_1 = ZoneTrigger(Trigger_1_info, 4, 4, 1, true)
	Trigger_1:SetPosition(27, 4, 9.5)
	Trigger_1:SetAction1(123)
	Trigger_1:SetAction2(-1)
	Trigger_1:SetAction3(-1)
	environment:AddTrigger(Trigger_1)

	Trigger_2_info = TriggerInfo(2, "exit21", true, true, false)
	Trigger_2 = ZoneTrigger(Trigger_2_info, 1, 5.5, 6, true)
	Trigger_2:SetPosition(0.5, 5, 34)
	Trigger_2:SetAction1(124)
	Trigger_2:SetAction2(-1)
	Trigger_2:SetAction3(-1)
	environment:AddTrigger(Trigger_2)

	Trigger_3_info = TriggerInfo(3, "exit25", true, true, false)
	Trigger_3 = ZoneTrigger(Trigger_3_info, 3, 3.5, 1, true)
	Trigger_3:SetPosition(62.5, 3, 0.5)
	Trigger_3:SetAction1(125)
	Trigger_3:SetAction2(-1)
	Trigger_3:SetAction3(-1)
	environment:AddTrigger(Trigger_3)

	Trigger_4_info = TriggerInfo(4, "exit26", true, true, false)
	Trigger_4 = ZoneTrigger(Trigger_4_info, 1, 3.5, 4, true)
	Trigger_4:SetPosition(25.5, 1, 51)
	Trigger_4:SetAction1(126)
	Trigger_4:SetAction2(-1)
	Trigger_4:SetAction3(-1)
	environment:AddTrigger(Trigger_4)

	Trigger_5_info = TriggerInfo(5, "exit27", true, true, false)
	Trigger_5 = ZoneTrigger(Trigger_5_info, 1, 4, 4, true)
	Trigger_5:SetPosition(5.5, 1, 20)
	Trigger_5:SetAction1(127)
	Trigger_5:SetAction2(-1)
	Trigger_5:SetAction3(-1)
	environment:AddTrigger(Trigger_5)

	Trigger_6_info = TriggerInfo(6, "exit28", true, true, false)
	Trigger_6 = ZoneTrigger(Trigger_6_info, 2, 2, 2, true)
	Trigger_6:SetPosition(24, 0, 58)
	Trigger_6:SetAction1(128)
	Trigger_6:SetAction2(-1)
	Trigger_6:SetAction3(-1)
	environment:AddTrigger(Trigger_6)

	Trigger_7_info = TriggerInfo(7, "exit30", true, true, false)
	Trigger_7 = ZoneTrigger(Trigger_7_info, 4, 4, 1, true)
	Trigger_7:SetPosition(5, 5, 6.5)
	Trigger_7:SetAction1(129)
	Trigger_7:SetAction2(-1)
	Trigger_7:SetAction3(-1)
	environment:AddTrigger(Trigger_7)

	Trigger_8_info = TriggerInfo(8, "exit33", true, true, false)
	Trigger_8 = ZoneTrigger(Trigger_8_info, 2, 3.5, 1, true)
	Trigger_8:SetPosition(1, 3, 49.5)
	Trigger_8:SetAction1(130)
	Trigger_8:SetAction2(-1)
	Trigger_8:SetAction3(-1)
	environment:AddTrigger(Trigger_8)

	Trigger_9_info = TriggerInfo(9, "exit34", true, true, false)
	Trigger_9 = ZoneTrigger(Trigger_9_info, 2, 3.5, 1, true)
	Trigger_9:SetPosition(1, 3, 50.5)
	Trigger_9:SetAction1(131)
	Trigger_9:SetAction2(-1)
	Trigger_9:SetAction3(-1)
	environment:AddTrigger(Trigger_9)

	Trigger_10_info = TriggerInfo(10, "exit35", true, true, false)
	Trigger_10 = ZoneTrigger(Trigger_10_info, 2, 3.5, 1, true)
	Trigger_10:SetPosition(1, 3, 51.5)
	Trigger_10:SetAction1(132)
	Trigger_10:SetAction2(-1)
	Trigger_10:SetAction3(-1)
	environment:AddTrigger(Trigger_10)

	Trigger_11_info = TriggerInfo(11, "exit36", true, true, false)
	Trigger_11 = ZoneTrigger(Trigger_11_info, 2, 3.5, 1, true)
	Trigger_11:SetPosition(1, 3, 52.5)
	Trigger_11:SetAction1(133)
	Trigger_11:SetAction2(-1)
	Trigger_11:SetAction3(-1)
	environment:AddTrigger(Trigger_11)

	Trigger_12_info = TriggerInfo(12, "exit38", true, true, false)
	Trigger_12 = ZoneTrigger(Trigger_12_info, 3, 3.5, 1, true)
	Trigger_12:SetPosition(30.5, 13, 4.5)
	Trigger_12:SetAction1(134)
	Trigger_12:SetAction2(-1)
	Trigger_12:SetAction3(-1)
	environment:AddTrigger(Trigger_12)

	Trigger_13_info = TriggerInfo(13, "exit43", true, true, false)
	Trigger_13 = ZoneTrigger(Trigger_13_info, 3, 3.5, 1, true)
	Trigger_13:SetPosition(21.5, 1, 14.5)
	Trigger_13:SetAction1(135)
	Trigger_13:SetAction2(-1)
	Trigger_13:SetAction3(-1)
	environment:AddTrigger(Trigger_13)

	Trigger_14_info = TriggerInfo(14, "exit8", true, true, false)
	Trigger_14 = ZoneTrigger(Trigger_14_info, 2, 4, 3, true)
	Trigger_14:SetPosition(6, 3, 62.5)
	Trigger_14:SetAction1(136)
	Trigger_14:SetAction2(-1)
	Trigger_14:SetAction3(-1)
	environment:AddTrigger(Trigger_14)

	Trigger_15_info = TriggerInfo(15, "exit9", true, true, false)
	Trigger_15 = ZoneTrigger(Trigger_15_info, 3, 4, 2, true)
	Trigger_15:SetPosition(8.5, 3, 62)
	Trigger_15:SetAction1(137)
	Trigger_15:SetAction2(-1)
	Trigger_15:SetAction3(-1)
	environment:AddTrigger(Trigger_15)

end
