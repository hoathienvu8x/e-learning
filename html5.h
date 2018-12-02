
#pragma once

#include <type_traits>
#include <memory>
#include <functional>

#include <string>
#include <vector>
#include <map>

namespace html{

	template<class T>
	class basic_dom;
	namespace detail { template<class T> class basic_dom_node_parser;}

	template<class T>
	class basic_selector
	{
	public:
		basic_selector(const std::basic_string<T>&);
		basic_selector(std::basic_string<T>&&);

		basic_selector(const T* s)
			: basic_selector(std::basic_string<T>(s))
		{}

		template<int N>
		basic_selector(const T s[N])
			: basic_selector(std::basic_string<T>(s))
		{};

		friend class basic_dom<T>;
		friend class detail::basic_dom_node_parser<T>;

	protected:
		struct condition
		{
			std::basic_string<T> matching_tag_name;
			std::basic_string<T> matching_id;
			std::basic_string<T> matching_class;
			std::basic_string<T> matching_name;
			std::basic_string<T> matching_index;
			std::basic_string<T> matching_attr;
			std::basic_string<T> matching_attr_value;
			std::basic_string<T> matching_attr_operator;

			bool operator()(const basic_dom<T>&, int&) const;
		};

		struct selector_matcher{
			bool operator()(const basic_dom<T>&) const;

		private:
			bool all_match = false;
			std::vector<condition> m_conditions;

			friend class basic_selector;
		};
		typedef typename std::vector<selector_matcher>::const_iterator selector_matcher_iterator;

		selector_matcher_iterator begin() const {
			return m_matchers.begin();
		}

		selector_matcher_iterator end() const {
			return m_matchers.end();
		}

	private:
		void build_matchers();

		std::vector<selector_matcher> m_matchers;

		std::basic_string<T> m_select_string;
	};

	enum tag_stage{
		tag_open,
		tag_close,
	};

	namespace detail {
		template<typename T>
		class basic_dom_node_parser
		{
			basic_dom_node_parser& operator = (basic_dom_node_parser&&)  = delete;
			basic_dom_node_parser& operator = (const basic_dom_node_parser&) = delete;

			basic_dom_node_parser(html::basic_dom<T>* domer, const std::basic_string<T>& str);
			basic_dom_node_parser(basic_dom_node_parser&& other);

		public: // only for signals2
			basic_dom_node_parser(const basic_dom_node_parser&);
			// called from dom
			void operator()(tag_stage, std::shared_ptr<basic_dom<T>>);

		public: // interface
			template<typename Handler>
			typename std::enable_if<
				std::is_same<typename std::result_of<Handler(tag_stage, std::shared_ptr<basic_dom<T>>)>::type, void>::value
			>::type
			operator |(const Handler& node_reciver)
			{
				set_callback_fuction(decltype(m_callback)(node_reciver));
			}

			template<typename Handler>
			typename std::enable_if<std::is_function<Handler>::value>::type
			operator |(const Handler& node_reciver)
			{
				set_callback_fuction(decltype(m_callback)(node_reciver));
			}

			basic_dom_node_parser& operator |(const basic_selector<T>&);

		public:
			~basic_dom_node_parser();

			friend class basic_dom<T>;
		private:

			void set_callback_fuction(std::function<void(tag_stage, std::shared_ptr<basic_dom<T>>)>&& cb);

			basic_dom<T>* m_dom;
			const basic_selector<T>* m_selector;

			const std::basic_string<T>& m_str;

			std::function<void(tag_stage, std::shared_ptr<basic_dom<T>>)> m_callback;
			boost::signals2::scoped_connection m_sig_connection;
		};
	}


	template<typename T>
	class basic_dom : public std::enable_shared_from_this<basic_dom<T>>
	{
	public:

		basic_dom(basic_dom<T>* parent = nullptr) noexcept;

		explicit basic_dom(const std::basic_string<T>& html_page, basic_dom<T>* parent = nullptr);

		explicit basic_dom(const basic_dom<T>& d);
		basic_dom(basic_dom<T>&& d);
		basic_dom<T>& operator = (const basic_dom<T>& d);
		basic_dom<T>& operator = (basic_dom<T>&& d);

	public:
		detail::basic_dom_node_parser<T> append_partial_html(const std::basic_string<T>&);

	public:
		basic_dom<T>  operator[](const basic_selector<T>&) const;

		std::basic_string<T> to_html() const;

		std::basic_string<T> to_plain_text() const;

		// return charset of the page if page contain meta http-equiv= content="charset="
		template<typename... Dummy, typename U = CharType>
		typename std::enable_if<std::is_same<U, char>::value, std::basic_string<T>>::type
		charset(const std::string& default_charset = "UTF-8") const
		{
			static_assert(sizeof...(Dummy)==0, "Do not specify template arguments!");
			return basic_charset(default_charset);
		}

		std::vector<std::shared_ptr<basic_dom<T>>> get_children(){
			return children;
		}

		std::basic_string<T> get_attr(const std::basic_string<T>& attr)
		{
			auto it = attributes.find(attr);

			if (it==attributes.end())
			{
				return std::basic_string<T>();
			}

			return it->second;
		}

	private:
		void html_parser(typename boost::coroutines::asymmetric_coroutine<const std::basic_string<T>*>::pull_type & html_page_source);
		typename boost::coroutines::asymmetric_coroutine<const std::basic_string<T>*>::push_type html_parser_feeder;
		bool html_parser_feeder_inialized = false;

		typedef std::shared_ptr<basic_dom<T>> basic_dom_ptr;
		boost::signals2::signal<void(tag_stage, basic_dom_ptr)> m_new_node_signal;

 		std::basic_string<T> basic_charset(const std::string& default_charset) const;

	protected:

		void to_html(std::basic_ostream<T>*, int deep) const;


		std::map<std::basic_string<T>, std::basic_string<T>> attributes;
		std::basic_string<T> tag_name;

		std::basic_string<T> content_text;
		std::vector<basic_dom_ptr> children;
		basic_dom<T>* m_parent;

		template<class T>
		static void dom_walk(basic_dom_ptr d, T handler);

		friend class basic_selector<T>;
		friend class detail::basic_dom_node_parser<T>;
	};

	typedef basic_dom<char> dom;
	typedef basic_dom<wchar_t> wdom;

} // namespace html
