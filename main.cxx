#include <bits/stdc++.h>

using namespace std;

namespace csv
{

class Parser
{
private:
    vector<string> m_title_fields;
    typedef vector<string> record_type;
    vector<record_type> m_data;

    struct string_trim_result
    { string::const_iterator begin, end; };

    static string_trim_result string_trim(string::const_iterator string_begin, string::const_iterator string_end);
    static record_type parse_line(string::const_iterator line_begin, string::const_iterator line_end);
public:
    Parser(string const& title_line)
        : m_title_fields(parse_line(title_line.begin(), title_line.end()))
        , m_data()
    {}
    void add_records( string const& data_str);
    vector<string> const& titles() const { return m_title_fields; }
    record_type const& record_at(size_t index) const { return m_data.at(index); }
    string const& field_at(size_t record_index, size_t field_index) const { return record_at(record_index).at(field_index); }
    size_t record_count() const { return m_data.size(); }
    size_t field_count() const { return m_title_fields.size(); }
};

Parser::string_trim_result 
Parser::string_trim(
    string::const_iterator string_begin, 
    string::const_iterator string_end
)
{
    typedef string::const_iterator Iterator;
    Iterator new_begin = string_begin, new_last = string_end - 1;
    while (new_begin != string_end && ! isgraph(*new_begin))
        ++new_begin;
    while (new_last != new_begin && ! isgraph(*new_last))
        --new_last;
    Parser::string_trim_result result;
    result.begin = new_begin;
    result.end = new_last + 1;
    return result;
}

Parser::record_type
Parser::parse_line(
    string::const_iterator line_begin, 
    string::const_iterator line_end
)
{
    typedef string::const_iterator Iterator;
    Parser::string_trim_result const trimmed = string_trim(line_begin, line_end);
    // Each field is enclosed in double quotes
    Parser::record_type fields;
    Iterator field_begin = trimmed.begin, field_end;
    do {
        field_begin =  find(field_begin, trimmed.end, '\"');
        field_end =  find(field_begin + 1, trimmed.end, '\"');
        if (field_begin == trimmed.end)
            break; // No more fields
        if (field_begin != trimmed.end && field_end == trimmed.end)
            throw runtime_error("Invalid CSV format: missing quote");
        fields.push_back(string(field_begin + 1, field_end));
        field_begin = field_end + 1;
    } while (true);
    return fields;
}

void 
Parser::add_records(
    string const& data_str
)
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
    while (record_end != trimmed.end) {
        record_begin = record_end + 1;
        record_end =  find(record_begin, trimmed.end, '\n');
        Parser::record_type const fields = parse_line(record_begin, record_end);
        if (fields.size() != this->field_count())
            throw runtime_error("Invalid CSV format: field count mismatch");
        m_data.push_back(fields);
    }
}

} // namespace csv

class Location
{
public:
    virtual ~Location() {}
    bool operator<(Location const& other) const { return (string)(*this) < (string)(other); }
    virtual operator string() const = 0;
    virtual string to_string() const = 0;
};
class FromLocation
    : public Location
{
protected:
    string city;
public:
    FromLocation(string const& city) 
        : city(city)
    {}
    virtual ~FromLocation() {}

    virtual operator string() const { return "{ " + this->city + " }"; }
    virtual string to_string() const
    { return "From: { " + this->city + " }"; }
};
class ToLocation
    : public Location
{
protected:
    string city;
public:
    ToLocation(string const& city) 
        : city(city) 
    {}
    virtual ~ToLocation() {}

    virtual operator string() const { return "{ " + this->city + " }"; }
    virtual string to_string() const
    { return "To: { " + this->city + " }"; }
};

typedef pair<FromLocation, ToLocation> Route;
Route
make_route(
    FromLocation from, 
    ToLocation to
)
{ return make_pair(from, to); }
string const distance_map_filename = "distance.csv";
class RouteToDistance
{
public:
    typedef Route RouteType;
    typedef unsigned int DistanceType;
protected:
    static map<RouteType, DistanceType> distance_map_init(string const& distance_map_file);
    static map<RouteType, DistanceType> const distance_map;
public:
    bool exists(RouteType const& route) const
    { return distance_map.find(route) != distance_map.end(); }
    DistanceType operator()(RouteType const& route) const
    { 
        try { return distance_map.at(route); }
        catch (out_of_range const&) { throw out_of_range("Route not found"); }
    }
} const route_to_distance;

map<RouteToDistance::RouteType, RouteToDistance::DistanceType> 
RouteToDistance::distance_map_init(
    string const& distance_map_filename
)
{
    map<RouteToDistance::RouteType, RouteToDistance::DistanceType> distance_map;
    ifstream distance_map_stream(distance_map_filename.c_str());
    if (!distance_map_stream)
        throw runtime_error("Failed to open distance map file");
    
    string distance_map_title_line;
    getline(distance_map_stream, distance_map_title_line);
    
    distance_map_stream.seekg(0);
    string file_buffer((istreambuf_iterator<char>(distance_map_stream)), istreambuf_iterator<char>());

    csv::Parser distance_map_parser(distance_map_title_line);
    distance_map_parser.add_records(file_buffer);

    for (size_t i = 0; i < distance_map_parser.record_count(); ++i) {
        string const& from_city = distance_map_parser.field_at(i, 0);
        string const& to_city = distance_map_parser.field_at(i, 1);
        RouteToDistance::RouteType const route = make_pair(FromLocation(from_city), ToLocation(to_city));
        istringstream ss(distance_map_parser.field_at(i, 2));
        RouteToDistance::DistanceType distance;
        ss >> distance;
        distance_map[route] = distance;
    }

    return distance_map;
}

map<RouteToDistance::RouteType, RouteToDistance::DistanceType> const RouteToDistance::distance_map = RouteToDistance::distance_map_init(distance_map_filename);

class Centimeter
{
public:
    typedef double ValueType;
private:
    ValueType value;
public:
    Centimeter(ValueType value)
        : value(value > 0 ? value : -1)
    {
        if (this->value == -1)
            throw runtime_error("Invalid centimeter value");
    }
    static string unit() { return "cm"; }
    operator ValueType() const { return this->value; }
};

class Freight
{
public:
    virtual ~Freight() {}

    virtual double volumetric_weight(Centimeter l, Centimeter w, Centimeter h, unsigned int packages) const = 0;
};
class AirFreight
    : public Freight
{
public:
    virtual ~AirFreight() {}

    virtual double volumetric_weight(Centimeter l, Centimeter w, Centimeter h, unsigned int packages) const
    { return l * w * h / 6000.0 * packages; }
} const air_freight;
class OceanFreight
    : public Freight
{
public:
    virtual ~OceanFreight() {}

    virtual double volumetric_weight(Centimeter l, Centimeter w, Centimeter h, unsigned int packages) const
    { return l * w * h / 1000.0 * packages; }
} const ocean_freight;
class RailFreight
    : public Freight
{
public:
    virtual ~RailFreight() {}

    virtual double volumetric_weight(Centimeter l, Centimeter w, Centimeter h, unsigned int packages) const
    { return l * w * h / 3000.0 * packages; }
} const rail_freight;

int main()
{
    typedef FromLocation From;
    typedef ToLocation To;
    typedef RouteToDistance::DistanceType DistanceType;
    typedef RouteToDistance::RouteType RouteType;
    RouteType const route = make_route(From("Shanghai"), To("Beijing"));
    DistanceType const distance = route_to_distance(route);
    cout << distance << endl;

    Freight const& freight = air_freight;
    double const weight = freight.volumetric_weight(50, 60, 70, 2);

    cout << weight << endl;

    return 0;
}
