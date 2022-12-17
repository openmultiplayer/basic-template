#include <sdk.hpp>
#include <Server/Components/Vehicles/vehicles.hpp>

struct BasicTemplate final : IComponent, PoolEventHandler<IVehicle>
{
private:
	ICore* core_ = nullptr;
	IVehiclesComponent* vehicles = nullptr;

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
		vehicles = components->queryComponent<IVehiclesComponent>();
		if (vehicles)
		{
			vehicles->getPoolEventDispatcher().addEventHandler(this);
		}
	}

	void onReady() override
	{
		// Fire events here at earliest.
	}

	void onPoolEntryCreated(IVehicle& entry) override
	{
		for (INetwork* network : core_->getNetworks())
		{
			INetworkQueryExtension* query = queryExtension<INetworkQueryExtension>(network);
			if (query)
			{
				query->addRule("vehicles", std::to_string(vehicles->count()));
			}
		}
	}

	void onPoolEntryDestroyed(IVehicle& entry) override
	{
		for (INetwork* network : core_->getNetworks())
		{
			INetworkQueryExtension* query = queryExtension<INetworkQueryExtension>(network);
			if (query)
			{
				query->addRule("vehicles", std::to_string(vehicles->count()));
			}
		}
	}

	void onFree(IComponent* component) override
	{
		// Invalidate vehicles pointer so it can't be used past this point.
		if (component == vehicles)
		{
			vehicles = nullptr;
		}
	}

	~BasicTemplate()
	{
		// Clean up what you did above.
		if (vehicles)
		{
			vehicles->getPoolEventDispatcher().removeEventHandler(this);
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
