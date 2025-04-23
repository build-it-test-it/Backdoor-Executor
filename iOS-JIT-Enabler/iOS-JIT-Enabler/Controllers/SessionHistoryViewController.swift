import UIKit

class SessionHistoryViewController: UIViewController {
    
    // MARK: - Properties
    
    private let jitService: JITService
    private let tableView: UITableView = {
        let tableView = UITableView()
        tableView.register(SessionCell.self, forCellReuseIdentifier: SessionCell.identifier)
        tableView.translatesAutoresizingMaskIntoConstraints = false
        return tableView
    }()
    
    private let refreshControl = UIRefreshControl()
    private var sessions: [Session] = []
    
    // MARK: - Initialization
    
    init(jitService: JITService) {
        self.jitService = jitService
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    // MARK: - Lifecycle Methods
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        setupUI()
        setupTableView()
        loadSessions()
    }
    
    // MARK: - UI Setup
    
    private func setupUI() {
        view.backgroundColor = .systemBackground
        title = "Session History"
        
        // Add subviews
        view.addSubview(tableView)
        
        // Set up constraints
        NSLayoutConstraint.activate([
            tableView.topAnchor.constraint(equalTo: view.safeAreaLayoutGuide.topAnchor),
            tableView.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            tableView.trailingAnchor.constraint(equalTo: view.trailingAnchor),
            tableView.bottomAnchor.constraint(equalTo: view.safeAreaLayoutGuide.bottomAnchor)
        ])
    }
    
    private func setupTableView() {
        tableView.delegate = self
        tableView.dataSource = self
        
        // Set up refresh control
        refreshControl.addTarget(self, action: #selector(refreshSessions), for: .valueChanged)
        tableView.refreshControl = refreshControl
    }
    
    // MARK: - Data Loading
    
    private func loadSessions() {
        refreshControl.beginRefreshing()
        
        jitService.getDeviceSessions { [weak self] result in
            guard let self = self else { return }
            
            self.refreshControl.endRefreshing()
            
            switch result {
            case .success(let sessions):
                self.sessions = sessions.sorted(by: { $0.startedAt > $1.startedAt })
                
                if self.sessions.isEmpty {
                    self.showEmptyState()
                } else {
                    self.hideEmptyState()
                }
                
                self.tableView.reloadData()
                
            case .failure(let error):
                self.showError(error)
                self.showEmptyState()
            }
        }
    }
    
    @objc private func refreshSessions() {
        loadSessions()
    }
    
    // MARK: - Empty State
    
    private func showEmptyState() {
        let emptyLabel = UILabel()
        emptyLabel.text = "No JIT sessions found"
        emptyLabel.textAlignment = .center
        emptyLabel.textColor = .gray
        emptyLabel.font = UIFont.systemFont(ofSize: 16)
        
        tableView.backgroundView = emptyLabel
    }
    
    private func hideEmptyState() {
        tableView.backgroundView = nil
    }
    
    // MARK: - Session Actions
    
    private func showSessionDetails(_ session: Session) {
        var detailsMessage = """
        App: \(session.appName)
        Bundle ID: \(session.bundleId)
        Status: \(session.statusDisplayText)
        Started: \(session.formattedStartTime)
        """
        
        if let method = session.method {
            detailsMessage += "\nMethod: \(method)"
        }
        
        if let completedAt = session.completedAt {
            let formatter = DateFormatter()
            formatter.dateStyle = .short
            formatter.timeStyle = .short
            detailsMessage += "\nCompleted: \(formatter.string(from: completedAt))"
        }
        
        showAlert(title: "Session Details", message: detailsMessage)
    }
    
    private func reactivateSession(_ session: Session) {
        // Create an App object from the session
        let app = App(bundleId: session.bundleId, name: session.appName, iconData: nil)
        
        let loadingIndicator = showLoadingIndicator()
        
        jitService.enableJIT(for: app) { [weak self] result in
            guard let self = self else { return }
            
            self.hideLoadingIndicator(indicator: loadingIndicator)
            
            switch result {
            case .success(let response):
                // Show success message
                self.showAlert(title: "JIT Enabled", message: response.message) {
                    // Launch the app
                    if !self.jitService.launchApp(withBundleId: app.bundleId) {
                        self.showAlert(title: "Launch Failed", message: "Could not launch the app. Please open it manually.")
                    }
                    
                    // Refresh the sessions list
                    self.loadSessions()
                }
                
            case .failure(let error):
                self.showError(error)
            }
        }
    }
}

// MARK: - UITableViewDelegate, UITableViewDataSource

extension SessionHistoryViewController: UITableViewDelegate, UITableViewDataSource {
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return sessions.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        guard let cell = tableView.dequeueReusableCell(withIdentifier: SessionCell.identifier, for: indexPath) as? SessionCell else {
            return UITableViewCell()
        }
        
        let session = sessions[indexPath.row]
        cell.configure(with: session)
        
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        
        let session = sessions[indexPath.row]
        showSessionDetails(session)
    }
    
    func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        return 70
    }
    
    func tableView(_ tableView: UITableView, trailingSwipeActionsConfigurationForRowAt indexPath: IndexPath) -> UISwipeActionsConfiguration? {
        let session = sessions[indexPath.row]
        
        // Only allow reactivation for completed or failed sessions
        if session.status == "completed" || session.status == "failed" || session.status == "expired" {
            let reactivateAction = UIContextualAction(style: .normal, title: "Reactivate") { [weak self] (_, _, completion) in
                guard let self = self else { return }
                
                self.reactivateSession(session)
                completion(true)
            }
            
            reactivateAction.backgroundColor = .systemBlue
            
            return UISwipeActionsConfiguration(actions: [reactivateAction])
        }
        
        return nil
    }
}