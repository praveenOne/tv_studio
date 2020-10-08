#include <functional>
#include <memory>
#include <map>

template <typename TCharacter, typename SceneType, typename TMove>
class Prosecution
{
private:
    TCharacter &_subject;
    TCharacter const &_target;
    bool _isEndOnCollision;
    std::shared_ptr<TMove> _currentMove;
    uint32_t _lastUpdate{0};
    std::map<HDirection, std::function<void()>> _setupDirections;

public:
    Prosecution(TCharacter &subject, const TCharacter &target, bool isEndOnCollision, std::map<HDirection, std::function<void()>> setupDirections)
    : _subject{subject}, _target{target}, _isEndOnCollision{isEndOnCollision}, _setupDirections{setupDirections} // initialization list, when we have reference we have to use it (references need to be initialize)
    {
        // "duck typing": "if it looks like a duck, sounds like a duck, walks like a duck, then it's a duck"
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        _subject.addUpdate([this](auto *scene) { return UpdateSubject(scene); });
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        // nowadays, we don't use "bind" but prefer a lambda like that before
        // _subject.addUpdate(std::bind(&Prosecution<TCharacter,SceneType>::UpdateSubject, this, std::placeholders::_1));
    }

    bool UpdateSubject(SceneType *scene)
    {
        // we need to follow the target, two options:
        // 1. we make the position of our subject closer to the target, or
        // 2. we create and maintain a HMove that will go towards the target

        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        // the character's level of dispersion will decide how fast we update the moves
        auto dispersionCount = _subject.dispersionCount();

        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        // when it's time to update a move, we will cancel the outstanding move and create a new one
        auto currentTime = scene->age();
        auto keep_will{true};
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        if ((currentTime - _lastUpdate) > dispersionCount)
        {
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
            auto direction{_subject._position.x < _target._position.x ? HDirection::right : HDirection::left};
            if (_currentMove)
            {
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
                if (_currentMove->direction() != direction)
                {
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
                    _currentMove->cancel();
                    _currentMove.reset();
                    keep_will = !_isEndOnCollision;
                }
            }
            else
            {
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
                auto factory{_subject.moveFactory()};
                auto setupPos {_setupDirections.find(direction)};
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
                if (setupPos != _setupDirections.end()) {
                    setupPos->second();
                }
                _currentMove = factory(direction, currentTime);
            }
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
            _lastUpdate = currentTime;
        }
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        return keep_will;
    }
};
