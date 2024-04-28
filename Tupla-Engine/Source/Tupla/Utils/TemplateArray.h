#pragma once

template<typename T, size_t MAXCOUNT>
class TemplateArray;
template<typename T>
class TemplateVector;

class ITemplateArray
{
public:
	virtual ~ITemplateArray() = default;
	virtual void Clear() = 0;
	virtual size_t Size() = 0;
};

class ITemplateVector
{
public:
	virtual ~ITemplateVector() = default;
	virtual void Clear() = 0;
	virtual size_t Size() = 0;
};

template<typename T, size_t MAXCOUNT>
class TemplateArray : public ITemplateArray
{
public:
	void Clear() override
	{
		myData = {};
	}

	size_t Size() override
	{
		return MAXCOUNT;
	}

private:
	std::array<T, MAXCOUNT> myData;
};

template<typename T>
class TemplateVector : public ITemplateVector
{
public:
	void Clear() override
	{
		myData.clear();
	}
	size_t Size() override
	{
		return myData.size();
	}

	const std::vector<T>& GetData() const { return myData; }
	std::vector<T>& GetData() { return myData; }
	operator std::vector<T>() { return myData; }

private:
	std::vector<T> myData;
};

class TemplatedVectorHolder
{
public:
	template<typename T>
	TemplateVector<T>& GetVector()
	{
		if(myRIDToData.contains(typeid(T).hash_code()))
		{
			return *std::static_pointer_cast<TemplateVector<T>>(myTemplatedVectors[myRIDToData[typeid(T).hash_code()]]);
		}

		myRIDToData[typeid(T).hash_code()] = myTemplatedVectors.size();
		myTemplatedVectors.push_back(Tupla::CreateRef<TemplateVector<T>>());
		return *std::static_pointer_cast<TemplateVector<T>>(myTemplatedVectors.back());
	}

	void Clear()
	{
		for (const auto& vec : myTemplatedVectors)
		{
			vec->Clear();
		}
	}

private:
	std::vector<Tupla::Ref<ITemplateVector>> myTemplatedVectors {};
	std::unordered_map<size_t, size_t> myRIDToData {};
};

// TODO: FINISH UP IN HERE!!!
class TemplatedArrayHolder
{

};