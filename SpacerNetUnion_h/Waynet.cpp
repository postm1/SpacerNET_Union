// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// Add your code here . . .

	void SpacerApp::ToggleWP()
	{
		if (pickedWaypoint != pickedWaypoint2nd && dynamic_cast<zCVobWaypoint*>(pickedWaypoint) && dynamic_cast<zCVobWaypoint*>(pickedWaypoint2nd))
		{
			zCWayNet* waynet = ogame->GetWorld()->GetWayNet();
			if (!waynet) return;

			zCWaypoint* wp1 = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(pickedWaypoint));
			zCWaypoint* wp2 = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(pickedWaypoint2nd));
			if (!wp1 || !wp2) return;
			if (wp1 == wp2) return;

			if (!waynet->HasWay(wp1, wp2))
			{
				waynet->CreateWay(wp1, wp2);
			}
			else
			{
				waynet->DeleteWay(wp1, wp2);
			}

			print.PrintWin("Toggle waynet...", GFX_RED);
		}
	}
	void SpacerApp::ConnectWP()
	{
		if (pickedWaypoint != pickedWaypoint2nd && dynamic_cast<zCVobWaypoint*>(pickedWaypoint) && dynamic_cast<zCVobWaypoint*>(pickedWaypoint2nd))
		{
			zCWayNet* waynet = ogame->GetWorld()->GetWayNet();
			if (!waynet) return;

			zCWaypoint* wp1 = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(pickedWaypoint));
			zCWaypoint* wp2 = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(pickedWaypoint2nd));
			if (!wp1 || !wp2) return;
			if (wp1 == wp2) return;

			if (!waynet->HasWay(wp1, wp2))
			{
				waynet->CreateWay(wp1, wp2);
			}
		}
	}

	void SpacerApp::DisconnectWP()
	{
		if (pickedWaypoint != pickedWaypoint2nd && dynamic_cast<zCVobWaypoint*>(pickedWaypoint) && dynamic_cast<zCVobWaypoint*>(pickedWaypoint2nd))
		{
			zCWayNet* waynet = ogame->GetWorld()->GetWayNet();
			if (!waynet) return;

			zCWaypoint* wp1 = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(pickedWaypoint));
			zCWaypoint* wp2 = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(pickedWaypoint2nd));
			if (!wp1 || !wp2) return;
			if (wp1 == wp2) return;

			if (waynet->HasWay(wp1, wp2))
			{
				waynet->DeleteWay(wp1, wp2);
			}
		}
	}


	static zSTRING lastWPName = "";
	static bool lastAddToNet = true;

	void SpacerApp::CreateNewWaypoint(zCVob* newvob, bool addToNet)
	{
		zVEC3 pos = ogame->GetCamera()->connectedVob->GetPositionWorld();
		zVEC3 dir = ogame->GetCamera()->connectedVob->GetAtVectorWorld().Normalize();

		newvob->SetPositionWorld(pos + dir * 200);

		selectedWpForCreateIsBlocked = true;
		InsertIntoWorld(newvob, NULL);

		//MessageBox(0, "0", 0, 0);


		if (theApp.turnWpMode == TurnWpMode::AGAINST_CAMERA)
		{
			newvob->SetHeadingWorld(pos);
			newvob->RotateLocalY(180);
		}

		if (theApp.turnWpMode == TurnWpMode::ON_CAMERA)
		{
			newvob->SetHeadingWorld(pos);
		}



		newvob->ResetXZRotationsWorld();
		newvob->Release();

		if (addToNet && selectedWaypointForNet)
		{
			//WriteLine("addToNet && selectedWaypointForNet");
			zCWayNet* waynet = ogame->GetWorld()->GetWayNet();

			if (waynet)
			{
				zCWaypoint* wp1 = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(selectedWaypointForNet));
				zCWaypoint* wp2 = waynet->SearchWaypoint(static_cast<zCVobWaypoint*>(newvob));

				//MessageBox(0, "1", 0, 0);

				if (wp1 && wp2 && wp1 != wp2)
				{

					if (!waynet->HasWay(wp1, wp2))
					{
						//print.PrintRed("Connect");
						waynet->CreateWay(wp1, wp2);
						//MessageBox(0, "2", 0, 0);
					}
				}

			}
		}

		if (selectedWaypointForNet)
		{
			selectedWaypointForNet->SetDrawBBox3D(FALSE);
		}

		selectedWpForCreateIsBlocked = false;
		theApp.SetSelectedVob(newvob, "CreateNewVob");
	}

	void SpacerApp::CreateNewWaypointFromButtonClick()
	{

		zSTRING className = "zCVobWaypoint";


		if (!dynamic_cast<zCVobWaypoint*>(pickedVob))
		{
			print.PrintRed(GetLang("UNION_NO_WAYPOINT"));
			return;
		}

		if (lastWPName.IsEmpty())
		{
			if (!pickedVob)
			{
				print.PrintWin(GetLang("UNION_NO_WAYPOINT_TEMPLATE"));
				return;
			}
			else
			{
				lastWPName = pickedVob->GetVobName();
			}

		}





		zCVob* newvob = dynamic_cast<zCVob*>(zCObject::CreateNewInstance(className));

		if (newvob)
		{

			newvob->SetVobName(zSTRING(GetNextWayPointName(lastWPName)));
			print.PrintWin(ToStr GetLang("UNION_WP_INSERT") + ToStr newvob->GetVobName());
			CreateNewWaypoint(newvob, lastAddToNet);
			zinput->ClearKey(MOUSE_LEFT);
			ClearLMB();
		}
	}

	void SpacerApp::CreateNewWaypointFromNet(CString classNamePtr, CString vobNamePtr, bool addToNet, bool autoGenerate)
	{
		zSTRING className = zSTRING(classNamePtr);
		zSTRING vobName = zSTRING(vobNamePtr);

		lastWPName = vobName;
		lastAddToNet = addToNet;

		zCVob* newvob = dynamic_cast<zCVob*>(zCObject::CreateNewInstance(className));

		if (newvob)
		{

			if (!autoGenerate)
			{
				newvob->SetVobName(vobName);
			}
			else
			{
				newvob->SetVobName(zSTRING(GetNextWayPointName(vobName)));
			}

			CreateNewWaypoint(newvob, addToNet);

			zinput->ClearKey(MOUSE_LEFT);
			ClearLMB();

		}
	}

	void RecalcWPBBox(zCVob* vob)
	{
		if (dynamic_cast<zCVobWaypoint*>(vob))
		{
			/*
			zTBBox3D BBox;
			BBox.mins = zVEC3(-100, -150, -100);
			BBox.maxs = zVEC3(+100, +150, +100);
			BBox.Translate(vob->GetPositionWorld());
			vob->SetBBox3DWorld(BBox);
			*/
		}
	}

	zSTRING OnWorldAnalysewaynet()
	{
		zSTRING result = "";
		zCWorld* world = ogame->GetWorld();
		if (world)
		{
			zCWayNet* waynet = world->GetWayNet();
			if (waynet)
			{
				result = waynet->CheckConsistency(0);
				if (!result.IsEmpty())
				{
					result = ToStr "Errors found: " + result;
					//AfxMessageBox(result.ToChar());
				}
				else
				{
					//AfxMessageBox("No errors found.");
				}
			}
		}

		return result;
	}
}