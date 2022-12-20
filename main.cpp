/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2022, open.mp team and contributors.
 */

#include <sdk.hpp>
#include <Server/Components/Vehicles/vehicles.hpp>

class BasicTemplate final : public IComponent, public PoolEventHandler<IVehicle>
{
private:
	ICore* core_ = nullptr;
	IVehiclesComponent* vehicles_ = nullptr;

	void updateVehicleCount()
	{
		for (INetwork* network : core_->getNetworks())
		{
			INetworkQueryExtension* query = queryExtension<INetworkQueryExtension>(network);
			if (query)
			{
				query->addRule("vehicles", std::to_string(vehicles_->count()));
			}
		}
	}

public:
	// https://open.mp/uid
	PROVIDE_UID(/* UID GOES HERE */);

	StringView componentName() const override
	{
		return "Basic Template";
	}

	SemanticVersion componentVersion() const override
	{
		return SemanticVersion(0, 0, 1, 0);
	}

	void onLoad(ICore* c) override
	{
		// Cache core, player pool here
		core_ = c;
		c->printLn("Basic component template loaded.");
	}

	void onInit(IComponentList* components) override
	{
		// Cache components, add event handlers here.
		vehicles_ = components->queryComponent<IVehiclesComponent>();
		if (vehicles_)
		{
			vehicles_->getPoolEventDispatcher().addEventHandler(this);
		}
	}

	void onReady() override
	{
		// Fire events here at earliest.
		updateVehicleCount();
	}

	void onPoolEntryCreated(IVehicle& entry) override
	{
		updateVehicleCount();
	}

	void onPoolEntryDestroyed(IVehicle& entry) override
	{
		updateVehicleCount();
	}

	void onFree(IComponent* component) override
	{
		// Invalidate vehicles_ pointer so it can't be used past this point.
		if (component == vehicles_)
		{
			vehicles_ = nullptr;
		}
	}

	~BasicTemplate()
	{
		// Clean up what you did above.
		if (vehicles_)
		{
			vehicles_->getPoolEventDispatcher().removeEventHandler(this);
		}
	}

	void free() override
	{
		// Deletes the component.
		delete this;
	}

	void reset() override
	{
		// Resets data when the mode changes.
	}
};

COMPONENT_ENTRY_POINT()
{
	return new BasicTemplate();
}
