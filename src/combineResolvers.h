#include "combineContext.h"
#include "validationError.h"

#include "./domain/rtc/resolvers.h"

class CombineResolvers
{
public:
  explicit CombineResolvers(CombineContext *);
  String execute(JsonObject);

private:
  static const byte HASH_SIZE = 1;

  CombineContext *context;
  Resolvers *mutation[HASH_SIZE];
  Resolvers *query[HASH_SIZE];
};

CombineResolvers::CombineResolvers(CombineContext *context):
  context(context)
  {
    query[0] = new query_date(context);

    mutation[0] = new mutation_date_save(context);
  };


String CombineResolvers::execute(JsonObject json) {
  if (json["method"] == "query") {
    for(int i = 0 ; i < HASH_SIZE; i++) {
      if (query[i]->getName() == json["topic"]) {
        return query[i]->resolve(json);
      }

      return (new TopicNotFoundError())->toJsonString();
    }
  }
  else if (json["method"] == "mutation") {
    for(int i = 0 ; i < HASH_SIZE; i++) {
      if (mutation[i]->getName() == json["topic"]) {
        return mutation[i]->resolve(json);
      }

      return (new TopicNotFoundError())->toJsonString();
    }
  }

  return (new InvalidMethodError())->toJsonString();
}