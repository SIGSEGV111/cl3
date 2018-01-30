#include "system_types.hpp"
#include "io_text_string.hpp"

namespace cl3
{
	namespace system
	{
		namespace trace
		{
			using namespace types;

			class TComponent;
			class TCategory;

			namespace _
			{
				CL3PUBF void Log(const char*, unsigned, const TComponent&, const TCategory&, io::text::string::TString);
			}

			/*
			 * logging (log-levels? categories? components? all?)
			 * memory & cpu & threads monitoring
			 * global backtrace on request
			 * stop/resume threads
			 */

			class TComponent
			{
				// parser
				// allocator
				// network layer
				protected:
					const char* const name;
					mutable usys_t n_hits;
					bool enabled;

				public:
					inline void Hit() const { this->n_hits++; }
					inline bool IsEnabled() const CL3_GETTER { return this->enabled; }
					inline void Enable() { this->enabled = true; }
					inline void Disable() { this->enabled = false; }

					CLASS TComponent(const char* name) : name(name), n_hits(0), enabled(false) {}
			};

			class TCategory
			{
				// inner loop
				// function call
				// parameters
				// intermediate results
				// ...
				protected:
					const char* const name;
					mutable usys_t n_hits;
					bool enabled;

				public:
					inline void Hit() const { this->n_hits++; }
					inline bool IsEnabled() const CL3_GETTER { return this->enabled; }
					inline void Enable() { this->enabled = true; }
					inline void Disable() { this->enabled = false; }

					CLASS TCategory(const char* name) : name(name), n_hits(0), enabled(false) {}
			};

			template<typename... TArg>
			inline void Log(const char* filename, unsigned line, const TComponent& component, const TCategory& category, const char* format, TArg... args)
			{
				component.Hit();
				category.Hit();
				if(CL3_UNLIKELY(component.IsEnabled() && category.IsEnabled()))
					_::Log(filename, line, component, category, io::text::string::TString::Format(format, typeinfo::features::forward(args)...));
			}

			#define CL3_LOGMSG(component, category, ...) cl3::system::trace::Log(__FILE__, __LINE__, (component), (category), __VA_ARGS__)
		}
	}
}
