/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2022, open.mp team and contributors.
 */

// Required for most of open.mp.
#include <sdk.hpp>

// Include the vehicle component information.
#include <Server/Components/Vehicles/vehicles.hpp>

// This should use an abstract interface if it is to be passed to other components.  Like the files
// in `<Server/Components/>` you would share only this base class and keep the implementation
// private.
class BasicTemplate final : public IComponent, public PoolEventHandler<IVehicle>
{
private:
	// Hold a reference to the main server core.
	ICore* core_ = nullptr;
	
	// Hold a reference to the vehicle component so methods in it can be called.
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
	// Visit https://open.mp/uid to generate a new unique ID.
	PROVIDE_UID(/* UID GOES HERE */);

	// When this component is destroyed we need to tell any linked components this it is gone.
	~BasicTemplate()
	{
		// Clean up what you did above.
		if (vehicles_)
		{
			vehicles_->getPoolEventDispatcher().removeEventHandler(this);
		}
	}

	// Implement the vehicle pool listener API.
	void onPoolEntryCreated(IVehicle& entry) override
	{
		updateVehicleCount();
	}

	void onPoolEntryDestroyed(IVehicle& entry) override
	{
		updateVehicleCount();
	}
	
	// Implement the main component API.
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
		core_->printLn("Basic component template loaded.");
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

	void onFree(IComponent* component) override
	{
		// Invalidate vehicles_ pointer so it can't be used past this point.
		if (component == vehicles_)
		{
			vehicles_ = nullptr;
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

// Automatically called when the compiled binary is loaded.
COMPONENT_ENTRY_POINT()
{
	return new BasicTemplate();
}
