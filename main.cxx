#include <bits/stdc++.h>

using namespace std;

namespace csv
{

/*!
 *  @brief  `csv::Parser` is the type that parse the CSV file and store the data in a structured way.
 */
class Parser
{
  private:
    /*!
     * @brief   `csv::Parser::record_type` is a vector of strings that represents the fields in a line in the CSV file.
     */
    typedef vector<string> record_type;
    /*!
     * @brief   `csv::Parser::title_type` is a vector of strings that represents the title fields in the CSV file.
     */
    typedef vector<string> title_type;
    /*!
     * @brief   `csv::Parser::m_title_fields` is a vector of strings that stores the title fields in the CSV file.
     */
    title_type m_title_fields;
    /*!
     * @brief   `csv::Parser::m_data` is a vector of `record_type` that stores the data in the CSV file.
     */
    vector<record_type> m_data;

    /*!
     * @brief   `csv::Parser::string_trim_result` is a struct that stores two iterators of the trimmed string.
     * @details It stores the `begin` and the `end` of the trimmed string.
     */
    struct string_trim_result
    {
        string::const_iterator begin, end;
    };
    /*!
     * @brief   `csv::Parser::string_trim` is a function that trims the string from the beginning and the end.
     * @details It trims the string from the beginning and the end by removing the leading and trailing whitespaces.
     * @param   string_begin The beginning of the string
     * @param   string_end The end of the string
     * @return  `csv::Parser::string_trim_result` The trimmed string
     */
    static string_trim_result string_trim(string::const_iterator string_begin, string::const_iterator string_end);
    /*!
     * @brief   `csv::Parser::parse_line` is a function that parses a line in the CSV file.
     * @details It parses a line in the CSV file and returns a vector of strings that represents the fields in the line.
     * @param   line_begin The beginning of the line
     * @param   line_end The end of the line
     * @return  `csv::Parser::record_type` The fields in the line
     * @throws  `std::runtime_error` If the field is not enclosed in double quotes
     */
    static record_type parse_line(string::const_iterator line_begin, string::const_iterator line_end);

  public:
    /*!
     * @brief   `csv::Parser::Parser` is a constructor that initializes the `csv::Parser` object.
     * @details It initializes the `csv::Parser` object with the title line in the CSV file.
     * @param   title_line The title line in the CSV file
     */
    Parser(string const &title_line) 
        : m_title_fields(parse_line(title_line.begin(), title_line.end()))
        , m_data()
    {}
    /*!
     * @brief   `csv::Parser::add_records` is a function that adds the records in the CSV file.
     * @details It adds the records in the CSV file by parsing the data string and storing the data in the structured way.
     *          It also checks if the title line in the CSV file matches the title fields in the `csv::Parser` object.
     * @param   data_str The string that contains the data in the CSV file
     */
    void add_records(string const &data_str);
    /*!
     * @brief   `csv::Parser::titles` is a function that returns the title fields in the CSV file.
     * @return  `csv::Parser::title_type const &` The title fields in the CSV file
     */
    title_type const &titles() const
    {
        return m_title_fields;
    }
    /*!
     * @brief   `csv::Parser::record_at` is a function that returns the record at the specified index.
     * @param   index The index of the record
     * @return  `csv::Parser::record_type const &` The record at the specified index
     * @throws  `std::out_of_range` If the index is more than `record_count()`
     */
    record_type const &record_at(size_t index) const
    {
        return m_data.at(index);
    }
    /*!
     * @brief   `csv::Parser::field_at` is a function that returns the field at the specified index in the record at the specified index.
     * @param   record_index The index of the record
     * @param   field_index The index of the field
     * @return  `std::string const &` The field at the specified index in the record at the specified index
     * @throws  `std::out_of_range` If the index is more than `record_count()` or `field_count()`
     */
    string const &field_at(size_t record_index, size_t field_index) const
    {
        return record_at(record_index).at(field_index);
    }
    /*!
     * @brief   `csv::Parser::record_count` is a function that returns the number of records in the CSV file.
     * @return  `std::size_t` The number of records in the CSV file
     */
    size_t record_count() const
    {
        return m_data.size();
    }
    /*!
     * @brief   `csv::Parser::field_count` is a function that returns the number of fields in the CSV file.
     * @return  `std::size_t` The number of fields in the CSV file
     */
    size_t field_count() const
    {
        return m_title_fields.size();
    }
};

Parser::string_trim_result Parser::string_trim(string::const_iterator string_begin, string::const_iterator string_end)
{
    typedef string::const_iterator Iterator;
    Iterator new_begin = string_begin, new_last = string_end - 1;
    while (new_begin != string_end && !isgraph(*new_begin))
        ++new_begin;
    while (new_last != new_begin && !isgraph(*new_last))
        --new_last;
    Parser::string_trim_result result;
    result.begin = new_begin;
    result.end = new_last + 1;
    return result;
}

Parser::record_type Parser::parse_line(string::const_iterator line_begin, string::const_iterator line_end)
{
    typedef string::const_iterator Iterator;
    Parser::string_trim_result const trimmed = string_trim(line_begin, line_end);
    // Each field is enclosed in double quotes
    Parser::record_type fields;
    Iterator field_begin = trimmed.begin, field_end;
    do
    {
        field_begin = find(field_begin, trimmed.end, '\"');
        field_end = find(field_begin + 1, trimmed.end, '\"');
        if (field_begin == trimmed.end)
            break; // No more fields
        if (field_begin != trimmed.end && field_end == trimmed.end)
            throw runtime_error("Invalid CSV format: missing quote");
        fields.push_back(string(field_begin + 1, field_end));
        field_begin = field_end + 1;
    } while (true);
    return fields;
}

void Parser::add_records(string const &data_str)
{
    typedef string::const_iterator Iterator;
    // Trim the data string
    Parser::string_trim_result const trimmed = string_trim(data_str.begin(), data_str.end());
    // Parse the title line
    Iterator record_begin = trimmed.begin;
    Iterator record_end = find(record_begin, trimmed.end, '\n');
    Parser::record_type const title_fields = parse_line(record_begin, record_end);
    if (title_fields != this->titles())
        throw runtime_error("Invalid CSV format: title line mismatch");
    // Parse the data lines
    while (record_end != trimmed.end)
    {
        record_begin = record_end + 1;
        record_end = find(record_begin, trimmed.end, '\n');
        Parser::record_type const fields = parse_line(record_begin, record_end);
        if (fields.size() != this->field_count())
            throw runtime_error("Invalid CSV format: field count mismatch");
        m_data.push_back(fields);
    }
}

} // namespace csv

namespace mail
{

/*!
 * @brief   `mail::Location` is an abstract class that represents a location.
 */
class Location
{
  public:
    /*!
     * @brief   `mail::Location::~Location` is a pure virtual destructor of the `mail::Location` class.
     * @details This ensures that the `mail::Location` class is an abstract class and won't cause memory leak.
     */
    virtual ~Location() = 0;
    /*!
     * @brief   `mail::Location::operator<` is a function that compares two locations.
     * @details It compares two locations by comparing their string representations. This is to enable the use of `std::map`.
     * @param   other The other location
     * @return  `bool` `true` if the current location is less than the other location, `false` otherwise
     */
    bool operator<(Location const &other) const
    {
        return (string)(*this) < (string)(other);
    }
    /*!
     * @brief   `mail::Location::operator==` is a function that compares two locations.
     * @details It compares two locations by comparing their string representations. This is to enable the use of `std::map`.
     * @param   other The other location
     * @return  `bool` `true` if the current location is equal to the other location, `false` otherwise
     */
    bool operator==(Location const &other) const
    {
        return (string)(*this) == (string)(other);
    }
    /*!
     * @brief   `mail::Location::operator string` is a pure virtual function that returns the string representation of the location.
     */
    virtual operator string() const = 0;
    /*!
     * @brief   `mail::Location::to_string` is a pure virtual function that returns the string representation of the location with more information that suitable for human reading.
     */
    virtual string to_string() const = 0;
};
/*!
 * @brief   `mail::Location::~Location` is using the default destructor created by the compiler.
 * @details This ensures that the use of `mail::Location` won't cause any linker error.
 */
Location::~Location() = default;
/*!
 * @brief   `mail::FromLocation` is a class that represents a location from a city.
 */
class FromLocation : public Location
{
  protected:
    /*!
     * @brief   `mail::FromLocation::city` is a string that represents the city of the location.
     */
    string city;

  public:
    /*!
     * @brief   `mail::FromLocation::FromLocation` is a constructor that initializes the `mail::FromLocation` object.
     */
    FromLocation(string const &city) 
        : city(city)
    {
    }
    /*!
     * @brief   `mail::FromLocation::~FromLocation` is using the default destructor created by the compiler.
     * @details This ensures that the use of `mail::FromLocation` won't cause any linker error or memory leakage.
     */
    virtual ~FromLocation() = default;
    /*!
     * @brief   `mail::FromLocation::operator string` is a function that returns the string representation of the location.
     */
    virtual operator string() const override
    {
        return "{ " + this->city + " }";
    }
    /*!
     * @brief   `mail::FromLocation::to_string` is a function that returns the string representation of the location with more information.
     */
    virtual string to_string() const override
    {
        return "From: { " + this->city + " }";
    }
};
/*!
 * @brief   `mail::ToLocation` is a class that represents a location to a city.
 */
class ToLocation : public Location
{
  protected:
    /*!
     * @brief   `mail::ToLocation::city` is a string that represents the city of the location.
     */
    string city;

  public:
    /*!
     * @brief   `mail::ToLocation::ToLocation` is a constructor that initializes the `mail::ToLocation` object.
     * @param   city The city of the location
     */
    ToLocation(string const &city) 
        : city(city)
    {
    }
    /*!
     * @brief   `mail::ToLocation::~ToLocation` is using the default destructor created by the compiler.
     * @details This ensures that the use of `mail::ToLocation` won't cause any linker error or memory leakage.
     */
    virtual ~ToLocation() = default;
    /*!
     * @brief   `mail::ToLocation::operator string` is a function that returns the string representation of the location.
     */
    virtual operator string() const override
    {
        return "{ " + this->city + " }";
    }
    /*!
     * @brief   `mail::ToLocation::to_string` is a function that returns the string representation of the location with more information.
     */
    virtual string to_string() const override
    {
        return "To: { " + this->city + " }";
    }
};
/*!
 * @brief   `Route` is a type that represents a route from a location to another location.
 */
typedef pair<FromLocation, ToLocation> Route;
/*!
 * @brief   `mail::make_route` is a function that creates a route from a location to another location.
 * @param   from The location from
 * @param   to The location to
 * @return  `mail::Route` The route from the location `from` to the location `to`
 */
Route make_route(FromLocation from, ToLocation to)
{
    return make_pair(from, to);
}
/*!
 * @brief   `mail::RouteToDistance` is a class that stores the distance between two locations and converts a route to its distance.
 */
class RouteToDistance
{
  public:
    typedef Route RouteType;
    typedef unsigned int DistanceType;

  protected:
    /*!
     * @brief   `mail::RouteToDistance::distance_map_init` is a function that reads the distance from file.
     * @return  `std::map<mail::RouteToDistance::RouteType, mail::RouteToDistance::DistanceType>` The distance map
     */
    static map<RouteType, DistanceType> distance_map_init();
    /*!
     * @brief   `mail::RouteToDistance::distance_map_filename` is a string that represents the filename of the distance map file in CSV format.
     */
    static string const distance_map_filename;
    /*!
     * @brief   `mail::RouteToDistance::distance_map` is a map that stores the distance between two locations.
     */
    static map<RouteType, DistanceType> const distance_map;

  public:
    /*!
     * @brief   `mail::RouteToDistance::exists` is a function that checks if the route exists in the distance map.
     * @param   route The route
     * @return  `bool` `true` if the route exists in the distance map, `false` otherwise
     */
    bool exists(RouteType const &route) const
    {
        return distance_map.find(route) != distance_map.end();
    }
    /*!
     * @brief   `mail::RouteToDistance::operator()` is a function that converts a route to its distance.
     * @param   route The route to convert
     * @return  `mail::RouteToDistance::DistanceType` The distance between the two locations in the route
     * @throws  `std::out_of_range` If the route does not exist in the distance map
     */
    DistanceType operator()(RouteType const &route) const
    {
        try
        {
            return distance_map.at(route);
        }
        catch (out_of_range const &)
        {
            throw out_of_range("Route not found");
        }
    }
} const route_to_distance;

map<RouteToDistance::RouteType, RouteToDistance::DistanceType> RouteToDistance::distance_map_init()
{
    map<RouteToDistance::RouteType, RouteToDistance::DistanceType> distance_map;
    ifstream distance_map_stream(distance_map_filename);
    if (!distance_map_stream)
        throw runtime_error("Failed to open distance map file");

    string distance_map_title_line;
    getline(distance_map_stream, distance_map_title_line);

    distance_map_stream.seekg(0);
    string file_buffer((istreambuf_iterator<char>(distance_map_stream)), istreambuf_iterator<char>());

    csv::Parser distance_map_parser(distance_map_title_line);
    distance_map_parser.add_records(file_buffer);

    for (size_t i = 0; i < distance_map_parser.record_count(); ++i)
    {
        string const &from_city = distance_map_parser.field_at(i, 0);
        string const &to_city = distance_map_parser.field_at(i, 1);
        RouteToDistance::RouteType const route = make_pair(FromLocation(from_city), ToLocation(to_city));
        istringstream ss(distance_map_parser.field_at(i, 2));
        RouteToDistance::DistanceType distance;
        ss >> distance;
        distance_map[route] = distance;
    }

    return distance_map;
}

const string RouteToDistance::distance_map_filename = "distance.csv";
const map<RouteToDistance::RouteType, RouteToDistance::DistanceType> RouteToDistance::distance_map =
    RouteToDistance::distance_map_init();

/*!
 * @brief   `mail::Centimeter` is a class that represents a length in centimeters.
 */
class Centimeter
{
  public:
    typedef long double ValueType;

  private:
    /*!
     * @brief   `mail::Centimeter::value` is a value that represents the length in centimeters.
     */
    ValueType value;

  public:
    /*!
     * @brief   `mail::Centimeter::Centimeter` is a constructor that initializes the `mail::Centimeter` object.
     * @param   value A value not less than 0 that represents the length in centimeters
     * @throws  `std::runtime_error` If the value is less than 0
     */
    Centimeter(ValueType value = 0) : value(value >= 0 ? value : -1)
    {
        if (this->value == -1)
            throw runtime_error("Invalid centimeter value");
    }
    /*!
     * @brief   `mail::Centimeter::unit` is a function that returns the unit of the length.
     * @return  `string` The unit of the length
     */
    static string unit()
    {
        return "cm";
    }
    /*!
     * @brief   `mail::Centimeter::operator ValueType` is a function that converts the `mail::Centimeter` object to its value.
     * @return  `mail::Centimeter::ValueType` The value of the `mail::Centimeter` object
     */
    operator ValueType() const
    {
        return this->value;
    }
};

/*!
 * @brief   `mail::Freight` is an abstract class that represents a freight.
 * @example 
 *  ```cpp
 *  Freight const *freight = &air_freight;
 *  long double volumetric_weight = freight->volumetric_weight(32, 24, 1, 1);
 *  ```
 */
class Freight
{
  public:
    /*!
     * @brief   `mail::Freight::~Freight` is a pure virtual destructor of the `mail::Freight` class.
     * @details This ensures that the `mail::Freight` class is an abstract class and won't cause memory leak.
     */
    virtual ~Freight() = 0;
    /*!
     * @brief   `mail::Freight::volumetric_weight` is a pure virtual function that calculates the volumetric weight of this freight.
     * @param   l The length of the package
     * @param   w The width of the package 
     * @param   h The height of the package
     * @param   packages The number of packages
     * @return  `long double` The volumetric weight of the freight
     */
    virtual long double volumetric_weight(Centimeter l, Centimeter w, Centimeter h, unsigned int packages) const = 0;
    /*!
     * @brief   `mail::Freight::operator string` is a pure virtual function that returns the string representation of the freight.
     */
    virtual operator string() const = 0;
};
/*!
 * @brief   `mail::Freight::~Freight` is using the default destructor created by the compiler.
 * @details This ensures that the use of `mail::Freight` won't cause any linker error or memory leakage.
 */
Freight::~Freight() = default;

class AirFreight : public Freight
{
  public:
    virtual ~AirFreight() = default;

    virtual long double volumetric_weight(Centimeter l, Centimeter w, Centimeter h, unsigned int packages) const override
    {
        return l * w * h / 6000.0 * packages;
    }
    virtual operator string() const override
    {
        return "Air transport";
    }
} const air_freight;

class OceanFreight : public Freight
{
  public:
    virtual ~OceanFreight() = default;

    virtual long double volumetric_weight(Centimeter l, Centimeter w, Centimeter h, unsigned int packages) const override
    {
        return l * w * h / 1000.0 * packages;
    }
    virtual operator string() const override
    {
        return "Ocean transport";
    }
} const ocean_freight;

class RailFreight : public Freight
{
  public:
    virtual ~RailFreight() = default;

    virtual long double volumetric_weight(Centimeter l, Centimeter w, Centimeter h, unsigned int packages) const override
    {
        return l * w * h / 3000.0 * packages;
    }
    virtual operator string() const override
    {
        return "Rail transport";
    }
} const rail_freight;

// Corresponding to the CSV file format
class PostalAddress
{
  protected:
    string m_user_type;
    string m_country;
    string m_postal_code;
    string m_location;

  public:
    PostalAddress() = default;
    PostalAddress(string ut, string c, string pc, string l)
    {
        m_user_type = ut;
        m_country = c;
        m_postal_code = pc;
        m_location = l;
    }

    string getUserType() const
    {
        return m_user_type;
    }

    void setUserType(string ut)
    {
        if (ut == "business" || ut == "private")
        {
            m_user_type = ut;
        }
        else
        {
            cout << "Invalid user type! Choose business or private." << endl;
        }
    }

    string getCountry() const
    {
        return m_country;
    }

    void setCountry(string c)
    {
        m_country = c;
    }

    string getPostalCode() const
    {
        return m_postal_code;
    }

    void setPostalCode(string pc)
    {
        m_postal_code = pc;
    }

    std::string getLocation() const
    {
        return m_location;
    }

    void setLocation(string l)
    {
        m_location = l;
    }

    void display()
    {
        std::cout << "Country: " << m_country << std::endl;
        std::cout << "Postal code: " << m_postal_code << std::endl;
        std::cout << "location: " << m_location << std::endl;
    }
};

// Other information
class UserInfo
{
  protected:
    std::string m_name;
    std::string m_email;
    std::string m_phone_number;

  public:
    UserInfo() = default;
    UserInfo(std::string name, std::string email, std::string phone_number)
    {
        std::string m_name = name;
        std::string m_email = email;
        std::string m_phone_number = phone_number;
    }
    std::string getName() const
    {
        return m_name;
    }

    void setName(const std::string &name)
    {
        m_name = name;
    }

    std::string getEmail() const
    {
        return m_email;
    }

    void setEmail(const std::string &email)
    {
        m_email = email;
    }

    std::string getPhoneNumber() const
    {
        return m_phone_number;
    }

    void setPhoneNumber(const std::string &phone_number)
    {
        m_phone_number = phone_number;
    }
    void display()
    {
        std::cout << "Name: " << m_name << std::endl;
        std::cout << "Email: " << m_email << std::endl;
        std::cout << "Phone number: " << m_phone_number << std::endl;
    }
};

// Pure storage struct, no need for any method
struct Dimension
{
    Centimeter length;
    Centimeter width;
    Centimeter height;
};

class ShipmentMode
{
  private:
    int choice;
    string userType;
    Dimension m_dimension;
    string ModeOfShipment;

  public:
    ShipmentMode() = default;
    ShipmentMode(int c, string ut)
    {
        choice = c;
        userType = ut;
        setshipmentMode(c);
    }
    void selectShipmentMode()
    {
        if (userType == "private")
        {
            std::cout << "Select Mode of Shipment: " << std::endl;
            std::cout << "1. Document (32 x 24 x 1 cm)" << std::endl;
            std::cout << "2. Moving Box (75 x 35 x 35 cm)" << std::endl;
            std::cout << "3. Packages" << std::endl;
            std::cout << "Enter choice: " << std::endl;
        }
        else if (userType == "business")
        {
            std::cout << "Select Mode of Shipment: " << std::endl;
            std::cout << "1. Pallet (110 x 110 cm)" << std::endl;
            std::cout << "2. Container" << std::endl;
            std::cout << "3. Cargo" << std::endl;
            std::cout << "Enter choice: " << std::endl;
        }
    }
    void setshipmentMode(int choice)
    {
        if (userType == "private")
        {
            switch (choice)
            {
            case 1:
                m_dimension.length = 32;
                m_dimension.width = 24;
                m_dimension.height = 1;
                ModeOfShipment = "document";
                break;
            case 2:
                m_dimension.length = 75;
                m_dimension.width = 35;
                m_dimension.height = 35;
                ModeOfShipment = "moving box";
            case 3:
                ModeOfShipment = "packages";
                break;
            default:
                std::cout << "Invalid choice!" << std::endl;
                break;
            }
        }
        else if (userType == "business")
        {
            switch (choice)
            {
            case 1:
                m_dimension.length = 110;
                m_dimension.width = 110;
                ModeOfShipment = "pallet";
                break;
            case 2:
                ModeOfShipment = "container";
                break;
            case 3:
                ModeOfShipment = "cargo";
                break;
            default:
                std::cout << "Invalid choice \n" << std::endl;
                break;
            }
            if (m_dimension.length)
                std::cout << "Suggested length: " << m_dimension.length;
            if (m_dimension.width)
                std::cout << "Suggested width: " << m_dimension.width;
            if (m_dimension.height)
                std::cout << "Suggested height: " << m_dimension.height;
        }
    }
};

class PackageInfo
{
  protected:
    Dimension m_dimension;
    long double m_weight;
    unsigned int m_quantity;

  public:
    PackageInfo() = default;
    PackageInfo(long double length, long double width, long double height, long double weight, unsigned int quantity)
    {
            m_dimension.length = length;
            m_dimension.width = width;
            m_dimension.height = height;
            m_weight = weight;
            m_quantity = quantity;
    }

    long double getLength() const
    {
        return m_dimension.length;
    }

    void setLength(long double length)
    {
        m_dimension.length = length;
    }

    long double getWidth() const
    {
        return m_dimension.width;
    }

    void setWidth(long double width)
    {
        m_dimension.width = width;
    }

    long double getHeight() const
    {
        return m_dimension.height;
    }

    void setHeight(long double height)
    {
        m_dimension.height = height;
    }

    long double getWeight() const
    {
        return m_weight;
    }

    void setWeight(long double weight)
    {
        m_weight = weight;
    }

    unsigned int getQuantity() const
    {
        return m_quantity;
    }

    void setQuantity(unsigned int quantity)
    {
        m_quantity = quantity;
    }
    void display() const
    {
        std::cout << "Weight:" << m_weight << std::endl;
        std::cout << "length: " << m_dimension.length << std::endl;
        std::cout << "Width: " << m_dimension.width << std::endl;
        std::cout << "Height " << m_dimension.height << std::endl;
        std::cout << "Quantity " << m_quantity << std::endl;
    }
};

class ShipmentInfo
{
  protected:
    UserInfo m_user;
    PostalAddress m_origin;
    PostalAddress m_destination;
    PackageInfo m_package;
    UserInfo m_consignee;
    Freight const *m_service_type;
    long double m_freight_weight;
    long double m_cost;

    static Freight const *service_type_to_freight(string const &service_type)
    {
        if (service_type == "Air transport")
        {
            return &air_freight;
        }
        if (service_type == "Ocean transport")
        {
            return &ocean_freight;
        }
        if (service_type == "Rail transport")
        {
            return &rail_freight;
        }
        throw runtime_error("Invalid service type");
    }

  public:
    ShipmentInfo() = default;
    ShipmentInfo(
        const PostalAddress& origin, 
        const PostalAddress& destination, 
        const PackageInfo& package,
        const UserInfo& user, 
        const UserInfo& consignee, 
        const std::string& service_type,
        long double /* For ABI compatibility */
    )
    {
        m_origin = origin;
        m_destination = destination;
        m_package = package;
        m_user = user;
        m_consignee = consignee;
        m_service_type = service_type_to_freight(service_type);
        m_freight_weight = m_service_type->volumetric_weight(
            m_package.getLength(), 
            m_package.getWidth(), 
            m_package.getHeight(), 
            m_package.getQuantity()
        );
    }

    // Getters
    PostalAddress getOrigin() const
    {
        return m_origin;
    }
    PostalAddress getDestination() const
    {
        return m_destination;
    }
    PackageInfo getPackage() const
    {
        return m_package;
    }
    UserInfo getUser() const
    {
        return m_user;
    }
    UserInfo getconsignee() const
    {
        return m_consignee;
    }
    string getServiceType() const
    {
        return (string)(*m_service_type);
    }
    long double getCost() const
    {
        return m_cost;
    }
    long double getFreightWeight() const
    {
        return m_freight_weight;
    }

    // Setters
    void setOrigin(const PostalAddress &o)
    {
        m_origin = o;
    }
    void setDestination(const PostalAddress &d)
    {
        m_destination = d;
    }
    void setPackage(const PackageInfo &p)
    {
        m_package = p;
    }
    void setconsignee(const UserInfo &s)
    {
        m_consignee = s;
    }
    void setServiceType(const string &st)
    {
        m_service_type = service_type_to_freight(st);
    }
    void setFreightWeight(long double /* For ABI compatibility */)
    {
        m_freight_weight = m_service_type->volumetric_weight(
            m_package.getLength(), 
            m_package.getWidth(), 
            m_package.getHeight(), 
            m_package.getQuantity()
        );
    }
    void setCost(long double c)
    {
        m_cost = c;
    }
    void display()
    {
        std::cout << "Account" << std::endl;
        m_user.display();
        std::cout << std::endl;
        std::cout << "Origin" << std::endl;
        m_origin.display();
        std::cout << std::endl;
        std::cout << "Destination" << std::endl;
        m_destination.display();
        std::cout << std::endl;
        std::cout << "consignee information" << std::endl;
        m_consignee.display();
        std::cout << std::endl;
        std::cout << "Shipment information" << std::endl;
        m_package.display();
        std::cout << std::endl;
        std::cout << " Mode of transport: " << (string)(*m_service_type) << std::endl;
        std::cout << std::endl;
        std::cout << " Freight weight: " << m_freight_weight << std::endl;
        std::cout << std::endl;
        std::cout << "Cost: " << m_cost << std::endl;
    }
};

void interface()
{
    std::cout << "********************* Ship now *********************" << std::endl;
    std::cout << "*******1.Please enter your account******************" << std::endl;
    std::cout << "****** 2.Enter your departure and destination ******" << std::endl;
    std::cout << "****** 3.Enter your service type *******************" << std::endl;
    std::cout << "****** 4.Describe your shipment ********************" << std::endl;
    std::cout << "****** 5.Get shipping prices ***********************" << std::endl;

    std::string usertype, username, useremail, usernumber, ocountry, opostalcode, ocity, consigneename,
        consigneeemail, consigneenumber, dcountry, dpostalcode, dcity, shipmentid, shipmentdate, servicetype;

    char confirm;
    int userChoice, type;
    unsigned int packages;
    while (true)
    {
        std::cout << "Account" << std::endl;
        std::cout << "I am shipping as a.... ( business | private  )" << std::endl;
        std::cin >> usertype;
        std::cout << "please enter your account:( name | email | phone number ) " << std::endl;
        std::cin >> username;
        std::cin >> useremail;
        std::cin >> usernumber;
        UserInfo user(username, useremail, usernumber);
        std::cout << "Login successful!" << std::endl;
        std::cout << std::endl;

        std::cout << "Origin and Destination" << std::endl;
        std::cout << "Please enter your origin: ( country | postal code | city ) ";
        std::cin >> ocountry;
        std::cin >> opostalcode;
        std::cin >> ocity;

        PostalAddress origin(usertype,ocountry, opostalcode, ocity);
        std::cout << "Please enter your destination: ( country | postal code | city ) ";
        std::cin >> dcountry;
        std::cin >> dpostalcode;
        std::cin >> dcity;
        std::cout << "Please enter the consignee information: ( name | email | phone number )" << std::endl;
        std::cin >> consigneename;
        std::cin >> consigneeemail;
        std::cin >> consigneenumber;
        UserInfo consignee(consigneename, consigneeemail, consigneenumber);
        PostalAddress destination(usertype,dcountry, dpostalcode, dcity);
        std::cout << std::endl;


        //Determine and recommend the type of transport service
        ShipmentMode shipment;
        shipment.selectShipmentMode();
        std::cout << "Enter your choice (1, 2, or 3): ";
        std::cin >> userChoice;
        shipment = ShipmentMode(userChoice, usertype);
        shipment.setshipmentMode(userChoice);

        //Package details
        long double length, width, height, weight, cost;
        unsigned int quantity;
        std::cout << "Package Description" << std::endl;
        std::cout << "Please enter the quantity of the package" << std::endl;
        std::cin >> quantity;
        std::cout << "Please enter your weight(kg): " << std::endl;
        std::cin >> weight;
        std::cout << "Please enter your length(cm): " << std::endl;
        std::cin >> length;
        std::cout << "Please enter your width(cm): " << std::endl;
        std::cin >> width;
        std::cout << "Please enter your height(cm): " << std::endl;
        std::cin >> height;
        PackageInfo package(length, width, height, weight, quantity);
        std::cout << endl;

        //Enter and determine the mode of transport and calculate the volume of transport
        double FreightWeight = 0;
        std::cout << "Mode of transport" << std::endl;
        std::cout << "Please enter a number to select your shipping type: ( Ocean transport(1) | Air transport(2) | "
            "Rail Freight(3)) ";

        while (true)
        {
            cin >> type;
            switch (type)
            {
            case 1:
                servicetype = "Ocean transport";
                FreightWeight = ocean_freight.volumetric_weight(length, width, height, quantity);
                break;
            case 2:
                servicetype = "Air transport";
                FreightWeight = air_freight.volumetric_weight(length, width, height, quantity);
                break;
            case 3:
                servicetype = "Rail transport";
                FreightWeight = rail_freight.volumetric_weight(length, width, height, quantity);
                break;
            default:
                cout << "Invalid input, please re-enter: ";
                continue;
            }
            break;
        }

        ShipmentInfo info(origin, destination, package, user, consignee, servicetype, FreightWeight);
        std::cout << std::endl;

        //Distance calculation
        typedef mail::FromLocation From;
        typedef mail::ToLocation To;
        typedef mail::Route Route;
        typedef mail::RouteToDistance::DistanceType Distance;
        Route const route = mail::make_route(From(ocity), To(dcity));
        Distance const distance = mail::route_to_distance(route);

        // need calculate Fees

        cost = 0;
        
        std::cout << "The shipping fee is: " << info.getCost() << std::endl;
        std::cout << std::endl;

        std::cout << "Details are as follows" << std::endl;
        info.display();

        // Confirm whether to pay
        std::cout << "Do you want to confirm the shipment? (y/n): ";
        std::cin >> confirm;
        if (confirm == 'y' || confirm == 'Y')
        {
            std::cout << "Shipment confirmed. Please proceed to payment." << std::endl;
            std::cout << "Payment successful. Thank you!" << std::endl;
        }
        else
        {
            std::cout << "Shipment not confirmed." << std::endl;
        }

        // Use again or exit
        std::cout << "Do you want to quit? (Y/N): " << std::endl;
        std::cin >> confirm;
        if (confirm == 'Y' || confirm == 'y')
        {
            std::cout << "Thank you for your use !" << std::endl;
            break;
        }
    }
}

} // namespace mail

int main()
{
    mail::interface();
    return 0;
}
